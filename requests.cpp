#include <cstring>
#include "fs_server.h"
#include "requests.h"
#include "Session.h"
#include "SessionManager.h"
#include "BlockManager.h"

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
