#include <cstring>
#include "requests.h"
#include "Session.h"
#include "SessionManager.h"

extern SessionManager sessionManager;

void sessionRequest(unsigned int seq, const char *username) {
    Session *newSesh = new Session(seq, username);

    unsigned int seshNum = newSesh->getNum();

    sessionManager.add(seshNum, newSesh);

    sendResponse(seshNum, seq);
}

void createRequest(unsigned int sesh, unsigned int seq, const char *pathname, const char *username, char type) {
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

    fs_inode *parent = getNodeFromPath(pathname);
    fs_direntry* direntry = findEmptyDirentry(parent);

    strcpy(direntry.name, name);
    direntry.inode_block = block;
}

void sendResponse(unsigned int /*sessionNumber*/, unsigned int /*sequenceNumber*/) {
    return;
}
