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

char *readRequest(const char* /*username*/, const Path &path, unsigned int block) {
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

void writeRequest(const char* /*username*/, const Path &path, unsigned int block, const char* data) {
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

    //need reader lock on inode
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

    disk_writeblock(block, &inode);

    //const char *name = path.getNameCString(path.depth());

    uint32_t parent_inode_blocknum = traversePath(path, path.depth() - 1);
    disk_readblock(parent_inode_blocknum, &inode);

    //TODO below here still needs work (mainly the stuff with finding the direntry block and stuff)
    //update direntry to store "block"

    //uint32_t direntry_blocknum

    //strcpy(direntry->name, name);
    //direntry->inode_block = block;

    //free(name);
}

void deleteRequest(const char * /*username*/, const Path &/*path*/) {
    //validate inputs
    //  check that session belong to user (done)
    //  check that sequence number is sequential (done)
    //
    //  check that path is valid
    //
    //  check that username owns file
    //  check that it has no files or subdirectories and not root directory
    //
    //get direntry of what we are deleting
    //add inode_block to freeBlocks queue
    //set inode_block to zero
    //idk if we need to change any names
    //
    //if we deleted the last direntry on the block we can deallocate the block
    
    uint32_t file_inode_blocknum = 0;
    fs_inode file_inode;

    //fs_direntry direntry;

    //direntry.inode_block = 0;

    //if directory data block is now empty we have to free
    
    for (uint32_t i = 0; i < file_inode.size; i++) {
        blockManager.freeBlock(file_inode.blocks[i]);
    }

    blockManager.freeBlock(file_inode_blocknum);
    //TODO need to write changes back to disk
}

void sendResponse(unsigned int /*sessionNumber*/, unsigned int /*sequenceNumber*/, const char * /*data*/) {
    return;
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
