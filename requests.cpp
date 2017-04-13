#include <cstring>
#include <cassert>
#include "fs_server.h"
#include "requests.h"
#include "Session.h"
#include "SessionManager.h"
#include "BlockManager.h"
#include "Path.h"
#include <iostream>
using std::cout;
using std::endl;
using std::string;
extern SessionManager sessionManager;
extern BlockManager blockManager;

void sessionRequest(Request *request) {
    Session *newSesh = new Session(request->getSequence(), request->getUsername().c_str());

    unsigned int seshNum = newSesh->getNum();

    sessionManager.add(seshNum, newSesh);
    request->setSession(seshNum);
}

void readRequest(Request *request) {
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

    Path *path = request->getPath();
    uint32_t file_inode_blocknum = traversePath(*path, path->depth());

    //TODO need reader lock on inode
    disk_readblock(file_inode_blocknum, &file_inode);

    if (file_inode.type != 'f') {
        //TODO handle
    }

    uint32_t data_block = file_inode.blocks[request->getBlock()];
    //TODO unlock inode


    //TODO need reader lock on data
    disk_readblock(data_block, request->getData());
    //TODO unlock data
}

void writeRequest(Request *request) {
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

    Path *path = request->getPath();
    uint32_t file_inode_blocknum = traversePath(*path, path->depth());

    //need reader lock on inode
    disk_readblock(file_inode_blocknum, &file_inode);

    if (file_inode.type != 'f') {
        //TODO handle error
    }

    uint32_t data_block = -1;
    bool added = false;

    unsigned block = request->getBlock();

    if (block < file_inode.size) {
        data_block = file_inode.blocks[block];
    } else if (file_inode.size == block) {
        data_block = blockManager.getFreeBlock();
        added = true;
    } else {
        //TODO handle error
    }

    disk_writeblock(data_block, request->getData());

    if (added) {
      file_inode.blocks[file_inode.size] = data_block;
      file_inode.size++;
      disk_writeblock(file_inode_blocknum, &file_inode);
    }
}

void createRequest(Request *request) {
    //check that username is the same as the one in sesh (done)
    //check seq number is valid (done)
    //check that path exits
    //check that there are free blocks

    uint32_t block = blockManager.getFreeBlock();
    fs_inode inode;

    inode.type = request->getType();
    strcpy(inode.owner, request->getUsername().c_str());
    inode.size = 0;

    //Write file inode to disk
    disk_writeblock(block, &inode);

    Path *path = request->getPath();
    uint32_t parent_inode_block = traversePath(*path, path->depth() - 1);

    disk_readblock(parent_inode_block, &inode);

    const char *name = path->getNameCString(path->depth() - 1);
    addDirentry(&inode, parent_inode_block, name, block);
}

void deleteRequest(Request *request) {
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

    Path *path = request->getPath();
    uint32_t file_inode_block = traversePath(*path, path->depth());
    uint32_t dir_inode_block = traversePath(*path, path->depth() - 1);
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
        for (uint32_t j = 0; j < FS_DIRENTRIES; j++) {
            if (!strcmp(block_buffer[j].name, file_name)) {
                //TODO cry
            }

            if (!block_buffer[j].inode_block) {
                found = true;
                direntry_idx = j;
            }
        }
    }

    //If an empty directory listing could not be found for a block,
    //a new block must be allocated. It is important that this block
    //be filled with 0's (or at least just inode_block) so they are known
    //to be free
    if (!found) {
        memset(block_buffer, 0, FS_BLOCKSIZE);

        data_block = blockManager.getFreeBlock();
        dir_inode->blocks[dir_inode->size++] = data_block;
    }

    //Writes file data to direntry
    strcpy(block_buffer[direntry_idx].name, file_name);
    block_buffer[direntry_idx].inode_block = file_block;

    disk_writeblock(data_block, block_buffer);

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
        for (uint32_t j = 0; j < FS_DIRENTRIES; j++) {
            if (block_buffer[j].inode_block == file_block) {
                block_buffer[j].inode_block = 0;
                direntry_idx = i;
                goto exit;
            }
        }
    }

exit:
    //If the direntry block is now empty, it must be removed from the directory inode
    for (uint32_t i = 0; i < FS_DIRENTRIES; i++) {
        if (block_buffer[i].inode_block != 0) empty = false;
    }

    //Updates the inode if a direntry block had to be freed. Since the block is no longer
    //in use, it can be freed without being updated on disk. If the block is still in use, it
    //is not freed but is written back to disk instead.
    if (empty) {
        for (uint32_t i = direntry_idx; i < dir_inode->size - 1; i++) {
            dir_inode->blocks[i] = dir_inode->blocks[i + 1];
        }

        dir_inode->size--;

        disk_writeblock(dir_inode_block, dir_inode);

        blockManager.freeBlock(data_block);
    } else {
        disk_writeblock(data_block, block_buffer);
    }
}

// creates the <sessionnumber> <sequencenumber><NULL>(<data> if readblock)
// returns size of this response
// expects caller to free the response
char* createResponse(Request *request, unsigned &response_size) {
    string response = std::to_string(request->getSession()) + " " +
                      std::to_string(request->getSequence());
    char* res;

    if(request->isReadRequest()) {
        assert(request->getData());
        string tmp(request->getData());
        response_size = response.size() + FS_BLOCKSIZE + 1;
        res = new char[response_size];

        unsigned i;
        for (i=0; i<response.size(); ++i) {
            res[i] = response[i];
        }
        res[i++] = '\0';

        unsigned k;
        for (k=0; k<FS_BLOCKSIZE; ++k) {
            res[i + k] = request->getData()[k];
        }
    }
    else {
        response_size = response.size() + 1;
        res = new char[response_size];
        strcpy(res, response.c_str());
    }
 
    return res;
}

//This file returns the block number of file at the specified depth of the path
// depth should be <= path.depth()
// NOTE: reader lock of the inode you want will be aquired when this returns
uint32_t traversePath(const Path &path, int depth) {

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
