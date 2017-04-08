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

char *readBlock(unsigned int /*session*/, unsigned int /*seq*/, const char* /*username*/, const char* /*path*/, unsigned int /*block*/) {
    //validate input
    //  session belongs to user
    //  sequence is sequential
    //  username exists maybe?
    //  path is valid
    //  file has enough blocks
    //
    //get inode of file
    //read the correct block
    //return char* to the data
    return NULL;
}

void writeBlock(unsigned int /*session*/, unsigned int /*seq*/, const char* /*username*/, const char* /*path*/, unsigned int /*block*/, const char* /*data*/) {
    //validate input
    //  session belongs to user
    //  sequence is sequential
    //  username exists maybe
    //  path is valid
    //  block refers to an existing block or one directly after the end
    //
    //get inode for file
    //get the correct block (or get a new one)
    //write the data
    //update the inode if a new block was added
}

void createRequest(unsigned int /*sesh*/, unsigned int /*seq*/, const char *pathname, const char *username, char type) {
    //check that username is the same as the one in sesh
    //check that path exits
    //check that there are free blocks
    //check seq number is valid

    uint32_t block = blockManager.getFreeBlock();
    fs_inode inode;
    inode.type = type;
    strcpy(inode.owner, username);
    inode.size = 0;

    disk_writeblock(block, &inode);

    char *name = getName(pathname);

    fs_inode *parent = getParentInode(pathname);
    fs_direntry* direntry = findEmptyDirentry(parent);

    strcpy(direntry->name, name);
    direntry->inode_block = block;

    free(name);
}

void deleteRequest(unsigned int /*sesh*/, unsigned int /*seq*/, const char * /*pathname*/, const char * /*username*/) {
    //validate inputs
    // make sure it has no files or subdirectories and not root directory
    //get direntry of what we are deleting
    //add inode_block to freeBlocks queue
    //set inode_block to zero
    //idk if we need to change any names
    //
    //if we deleted the last direntry on the block we can deallocate the block

}

void sendResponse(unsigned int /*sessionNumber*/, unsigned int /*sequenceNumber*/) {
    return;
}

char *getName(const char *pathname) {
// path name is /nick/stuff/porn/video
// should return video
// you are responsible for freeing name

    int nameStart = 0;
    int i = 0;
    while (pathname[i]) {
        if(pathname[i] == '/') {
            nameStart = i + 1;
        }
        ++i;
    }

    char *name = strdup(pathname + nameStart);
    return name;
}

fs_inode *getParentInode(const char * /*pathname*/) {
// pathname is /nick/stuff/porn/video
// should return inode of porn

    fs_inode dir;
    disk_readblock(0, &dir);

    
    return new fs_inode();
}

fs_direntry *findEmptyDirentry(fs_inode * /*inode*/) {
    return new fs_direntry();
}

//This file returns the block number of file at the specified depth of the path
// depth should be <= path.depth()
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
