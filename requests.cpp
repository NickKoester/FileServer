#include <cstring>
#include "fs_server.h"
#include "requests.h"
#include "Session.h"
#include "SessionManager.h"
#include "BlockManager.h"
#include "Path.h"

extern SessionManager sessionManager;
extern BlockManager blockManager;

unsigned int sessionRequest(unsigned int seq, const char *username) {
    Session *newSesh = new Session(seq, username);

    unsigned int seshNum = newSesh->getNum();

    sessionManager.add(seshNum, newSesh);
    return seshNum;
}

char *readBlock(const char* /*username*/, const Path &path, unsigned int block) {
    //validate input
    //  session belongs to user (done)
    //  sequence is sequential (done)
    //  user owns file
    //  path is valid
    //  file has enough blocks
    //
    //get inode of file
    //read the correct block
    //return char* to the data
    
    fs_inode file_inode;
    uint32_t file_inode_blocknum = traversePath(path, path.depth());

    //TODO need reader lock on inode
    disk_readblock(file_inode_blocknum, &file_inode);

    if (file_inode.type != 'f') {
        //TODO handle
    }

    uint32_t data_block = file_inode.blocks[block];
    //TODO unlock inode
    char *buffer = new char[FS_BLOCKSIZE];

    //TODO need reader lock on data
    disk_readblock(data_block, buffer);
    //TODO unlock data

    return buffer;
}

void writeBlock(const char* /*username*/, const Path &path, unsigned int block, const char* data) {
    //validate input
    //  session belongs to user -> easy to make a function to check (done)
    //  sequence is sequential -> make function to check (done)
    //
    //  path is valid -> done during the traversal
    //
    //  username owns file -> must be done after the tree is traversed
    //  block refers to an existing block or one directly after the end -> done after traversal
    //
    //get inode for file
    //get the correct block (or get a new one)
    //write the data
    //update the inode if a new block was added

    fs_inode file_inode;
    uint32_t file_inode_blocknum = traversePath(path, path.depth());

    //TODO need reader lock on inode
    disk_readblock(file_inode_blocknum, &file_inode);

    if (file_inode.type != 'f') {
        //TODO handle error
    }

    uint32_t data_block = -1;
    bool added = false;

    if (file_inode.size < block) {
        data_block = file_inode.blocks[block];
    } else if (file_inode.size == block) {
        data_block = blockManager.getFreeBlock();
        added = true;
    } else {
        //TODO handle error
    }

    disk_writeblock(data_block, data);

    if (added) {
      file_inode.blocks[file_inode.size] = data_block;
      file_inode.size++;
      disk_writeblock(file_inode_blocknum, &file_inode);
    }
}

void createRequest(const char *username, const Path &path, char type) {
    //check that username is the same as the one in sesh (done)
    //check seq number is valid (done)
    //check that path exits
    //check that there are free blocks

    uint32_t block = blockManager.getFreeBlock();
    fs_inode inode;

    inode.type = type;
    strcpy(inode.owner, username);
    inode.size = 0;

    //Write file inode to disk
    disk_writeblock(block, &inode);

    uint32_t parent_inode_block = traversePath(path, path.depth() - 1);
    disk_readblock(parent_inode_block, &inode);

    const char *name = path.getNameCString(path.depth());
    addDirentry(&inode, parent_inode_block, name, block);
}

void deleteRequest(const char * /*username*/, const Path &path) {
    //validate inputs
    //  check that session belong to user (done)
    //  check that sequence number is sequential (done)
    //
    //  check that path is valid
    //
    //  check that username owns file
    //  check that it has no files or subdirectories and not root directory
    //
    //directory inode
    //  remove direntry
    //  check if direntry block is empty
    //  if it is, shift the rest down
    //  decrement size
    //  write inode to disk
    //
    //STATE: inode good but file still "exists"
    //
    //file inode
    //  free all data blocks
    //  free inode block
    //  no need to write to disk
    //
    //STATE: done

    uint32_t file_inode_block = traversePath(path, path.depth());
    uint32_t dir_inode_block = traversePath(path, path.depth() - 1);
    fs_inode inode;

    disk_readblock(dir_inode_block, &inode);

    removeDirentry(&inode, dir_inode_block, file_inode_block);

    disk_readblock(file_inode_block, &inode);
    
    for (uint32_t i = 0; i < inode.size; i++) {
        blockManager.freeBlock(inode.blocks[i]);
    }

    blockManager.freeBlock(file_inode_block);
}

//Adds information about newly created file to the directory refered to by dir_inode. This function finds a free direntry to store
//the information in and reserves the right to modify dir_inode if a new block needs to be allocated
void addDirentry(fs_inode *dir_inode, const uint32_t dir_inode_block, const char *file_name, const uint32_t file_block) {
    uint32_t data_block, direntry_idx = 0;
    bool found = false;
    char buffer[FS_BLOCKSIZE];
    fs_direntry *block_buffer = (fs_direntry *) buffer;

    //Checks for free direntry in each data block of the directory
    for (uint32_t i = 0; i < dir_inode->size; i++) {
        data_block = dir_inode->blocks[i];
        disk_readblock(data_block, block_buffer);

        //Checks if there is a free direntry entry in current block, where
        //a free direntry is one where inode_block is 0
        for (uint32_t j = 0; j < DIRENTRIES_PER_BLOCK; j++) {
            if (!block_buffer[j].inode_block) {
                found = true;
                direntry_idx = j;
                goto found;
            }
        }
    }

    //If an empty directory listing could not be found for a block,
    //a new block must be allocated. It is important that this block
    //be filled with 0's (or at least just inode_block) so they are known
    //to be free
    memset(block_buffer, 0, FS_BLOCKSIZE);

    data_block = blockManager.getFreeBlock();
    dir_inode->blocks[dir_inode->size++] = data_block;

    //Writes file data to direntry
found:
    strcpy(block_buffer[direntry_idx].name, file_name);
    block_buffer[direntry_idx].inode_block = file_block;

    disk_writeblock(data_block, &block_buffer);

    //Inode needs to be updated after the the data block is written to disk to
    //maintain a consistent state
    if (!found) {
        disk_writeblock(dir_inode_block, dir_inode);
    }
}

//Deletes the direntry containing file_block from dir_inode. May edit dir_inode if removing the entry causes
//the block to no longer be used
void removeDirentry(fs_inode *dir_inode, const uint32_t dir_inode_block, const uint32_t file_block) {
    uint32_t data_block, direntry_idx = 0;
    char buffer[FS_BLOCKSIZE];
    fs_direntry *block_buffer = (fs_direntry *) buffer;
    bool empty = true;

    //Checks for direntry in each data block of the directory
    for (uint32_t i = 0; i < dir_inode->size; i++) {
        data_block = dir_inode->blocks[i];
        disk_readblock(data_block, block_buffer);

        //Checks if the direntry is in the block in block_buffer
        for (uint32_t j = 0; j < DIRENTRIES_PER_BLOCK; j++) {
            if (block_buffer[j].inode_block == file_block) {
                block_buffer[j].inode_block = 0;
                direntry_idx = i;
                goto exit;
            }
        }
    }

exit:
    //If the direntry block is now empty, it must be removed from the directory inode
    for (uint32_t i = 0; i < DIRENTRIES_PER_BLOCK; i++) {
        if (block_buffer[i].inode_block != 0) empty = false;
    }

    if (empty) {
        for (uint32_t i = direntry_idx; i < dir_inode->size - 1; i++) {
            dir_inode->blocks[i] = dir_inode->blocks[i + 1];
        }
    }

    dir_inode->size = dir_inode->size - 1;

    disk_writeblock(dir_inode_block, dir_inode);

    blockManager.freeBlock(data_block);
}

void sendResponse(unsigned int /*sessionNumber*/, unsigned int /*sequenceNumber*/) {
    return;
}

//This file returns the block number of file at the specified depth of the path
// depth should be <= path.depth()
// NOTE: reader lock of the inode you want will be aquired when this returns
uint32_t traversePath(const Path &path, int depth) {
   //for every level of tree
   //  lock parent inode
   //  get parent inode
   //  get name of next file
   //  find child block
   //  get child inode
   //  lock child inode
   //  unlock parent inode

    if(depth > path.depth()) {
        //bad shit
    }
    uint32_t parentBlock = 0;
    uint32_t childBlock = 0;

    //TODO: get read lock for root directory

    for(int i = 0; i < depth; i++) {
        fs_inode parentDirectory;
        disk_readblock(parentBlock, &parentDirectory);

        const char *childName = path.getNameCString(i);
        childBlock = findBlock(&parentDirectory, childName);
        
        //TODO: get lock for child
        //TODO: release lock for parent

        parentBlock = childBlock;
    }
    return childBlock;
}

// This function finds the block that the file "name" is stored
// requires the parent directory and the filename to find it
uint32_t findBlock(fs_inode *parent, const char *name) {
    for (uint32_t i = 0; i < parent->size; i++) {
        uint32_t block = parent->blocks[i];
        fs_direntry direntries[FS_DIRENTRIES];
        disk_readblock(block, &direntries);
 
        for(uint32_t j = 0; j < FS_DIRENTRIES; j++) {
            fs_direntry direntry = direntries[j];
            if(!strcmp(direntry.name, name)) {
                return direntry.inode_block;
            }
        }
    }
    return 0;
}
