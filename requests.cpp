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
