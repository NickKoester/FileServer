#include <cstring>
#include <cassert>
#include "fs_server.h"
#include "requests.h"
#include "Session.h"
#include "SessionManager.h"
#include "BlockManager.h"
#include "LockManager.h"
#include "Path.h"
#include <iostream>
using std::cout;
using std::endl;
using std::string;
extern SessionManager sessionManager;
extern BlockManager blockManager;
extern LockManager lockManager;

void sessionRequest(Request *request) {
    Session *newSesh = new Session(request->getSequence(), request->getUsername().c_str());

    unsigned int seshNum = newSesh->getNum();

    sessionManager.add(seshNum, newSesh);
    request->setSession(seshNum);
}

void readRequest(Request *request) {
    fs_inode file_inode;

    Path *path = request->getPath();

    uint32_t file_inode_blocknum;
    try {
        //this function returns with the reader lock of file
        //unless exception is thrown
        file_inode_blocknum = traversePath(request, *path, path->depth(), false);
    } catch (std::runtime_error &e) {
        throw e;
    }

    disk_readblock(file_inode_blocknum, &file_inode);

    if (file_inode.type != 'f') {
        lockManager.releaseReadLock(file_inode_blocknum);
        throw std::runtime_error("Cannot read from directory\n");
    }

    if (!request->isOwner(file_inode)) {
        lockManager.releaseReadLock(file_inode_blocknum);
        throw std::runtime_error("You do not own this file\n");
    }

    if (file_inode.size <= request->getBlock()) {
        lockManager.releaseReadLock(file_inode_blocknum);
        throw std::runtime_error("Invalid read\n");
    }

    uint32_t data_block = file_inode.blocks[request->getBlock()];

    disk_readblock(data_block, request->getData());

    //releases lock that was aquired in the traversePath function
    lockManager.releaseReadLock(file_inode_blocknum);
}

void writeRequest(Request *request) {
    unsigned block = request->getBlock();

    fs_inode file_inode;

    Path *path = request->getPath();

    uint32_t file_inode_blocknum;
    try {
        //writer lock will be aquired from this function
        //unless exception is thrown
        file_inode_blocknum = traversePath(request, *path, path->depth(), true);
    } catch (std::runtime_error &e) {
        throw e;
    }

    disk_readblock(file_inode_blocknum, &file_inode);

    if (file_inode.type != 'f') {
        lockManager.releaseWriteLock(file_inode_blocknum);
        throw std::runtime_error("Cannot write to directory\n");
    }

    if (!request->isOwner(file_inode)) {
        lockManager.releaseWriteLock(file_inode_blocknum);
        throw std::runtime_error("You do not own this file\n");
    }

    uint32_t data_block = -1;
    bool added = false;

    if (block < file_inode.size) {
        data_block = file_inode.blocks[block];
    } else if (file_inode.size == block) {
        try {
            data_block = blockManager.getFreeBlock();
        } catch (std::runtime_error &e) {
            throw e;
        }
        added = true;
    } else {
        lockManager.releaseWriteLock(file_inode_blocknum);
        throw std::runtime_error("Writes must occur sequentially\n");
    }

    disk_writeblock(data_block, request->getData());

    if (added) {
      file_inode.blocks[file_inode.size] = data_block;
      file_inode.size++;
      disk_writeblock(file_inode_blocknum, &file_inode);
    }

    lockManager.releaseWriteLock(file_inode_blocknum);
}

void createRequest(Request *request) {
    fs_inode new_inode, parent_inode;
    uint32_t file_block;
    try {
        file_block = blockManager.getFreeBlock();
    } catch (std::runtime_error &e) {
        throw e;
    }

    new_inode.type = request->getType();
    strcpy(new_inode.owner, request->getUsername().c_str());
    new_inode.size = 0;

    Path *path = request->getPath();

    uint32_t parent_inode_block;
    try {
        //writer lock is aquired for this directory
        //unless exception is thrown
        parent_inode_block = traversePath(request, *path, path->depth() - 1, true);
    } catch (std::runtime_error &e) {
        throw e;
    }
    
    disk_readblock(parent_inode_block, &parent_inode);

    if (parent_inode.type != 'd') {
        string message = string("\"")                           +
                         path->getNameString(path->depth() - 2) +
                         string("\" is not a directory\n");
        blockManager.freeBlock(file_block);
        lockManager.releaseWriteLock(parent_inode_block);
        throw std::runtime_error(message);
    }

    //checks that we own directory,  short circuits if parent_inode_block is 0
    if (parent_inode_block && !request->isOwner(parent_inode)) {
        blockManager.freeBlock(file_block);
        lockManager.releaseWriteLock(parent_inode_block);
        throw std::runtime_error("You do not own this directory\n");
    }

    const char *name = path->getNameCString(path->depth() - 1);

    uint32_t direntry_block = 0;
    uint32_t direntry_idx   = 0;
    bool found = false;
    fs_direntry direntries[FS_DIRENTRIES];

    try {
        found = findEmptyDirentry(&parent_inode, name, &direntry_block, &direntry_idx, direntries);
    } catch (std::runtime_error &e) {
        blockManager.freeBlock(file_block);
        lockManager.releaseWriteLock(parent_inode_block);
        throw e;
    }

    //create new lock for this file and aquire it
    lockManager.createLock(file_block);
    lockManager.aquireWriteLock(file_block);

    disk_writeblock(file_block, &new_inode);

    //If an empty directory listing could not be found for a block,
    //a new block must be allocated. It is important that this block
    //be filled with 0's (or at least just inode_block) so they are known
    //to be free
    if (!found) {
        if (parent_inode.size == FS_MAXFILEBLOCKS) {
            throw std::runtime_error("Directory is full\n");
        }

        memset(direntries, 0, FS_DIRENTRIES * sizeof(fs_direntry));

        try {
            direntry_block = blockManager.getFreeBlock();
        } catch (std::runtime_error &e) {
            throw e;
        }

        parent_inode.blocks[parent_inode.size++] = direntry_block;
    }

    //Writes file data to direntry
    strcpy(direntries[direntry_idx].name, name);
    direntries[direntry_idx].inode_block = file_block;

    disk_writeblock(direntry_block, &direntries);

    //Inode needs to be updated after the the data block is written to disk to
    //maintain a consistent state
    if (!found) {
        disk_writeblock(parent_inode_block, &parent_inode);
    }

    lockManager.releaseWriteLock(parent_inode_block);
    lockManager.releaseWriteLock(file_block);
}

void deleteRequest(Request *request) {
    Path *path = request->getPath();

    if (!path->depth()) {
        throw std::runtime_error("Unable to delete root directory\n");
    }

    const char *filename = path->getNameCString(path->depth() - 1);

    uint32_t parent_inode_block;
    try {
        //writer lock aquired on dir_inode_block
        //unless exception is thrown
        parent_inode_block = traversePath(request, *path, path->depth() - 1, true);
    } catch (std::runtime_error &e) {
        throw e;
    }

    fs_inode parent_inode, file_inode;
    fs_direntry direntries[FS_DIRENTRIES];

    disk_readblock(parent_inode_block, &parent_inode);

    //checks that we own this file -- short circuits if root directory
    if (parent_inode_block && !request->isOwner(parent_inode)) {
        lockManager.releaseWriteLock(parent_inode_block);
        throw std::runtime_error("You do not own this directory\n");
    }

    uint32_t file_inode_block = 0;
    uint32_t direntry_idx = 0;
    uint32_t direntry_block = 0;

    try {
        file_inode_block = findExistingDirentry(&parent_inode, filename, &direntry_idx,  &direntry_block, direntries);
    } catch (std::runtime_error &e) {
        lockManager.releaseWriteLock(parent_inode_block);
        throw e;
    }

    //writer lock aquired on file
    lockManager.aquireWriteLock(file_inode_block);

    disk_readblock(file_inode_block, &file_inode);

    //check that directory is empty before delete
    if (file_inode.type == 'd' && file_inode.size != 0) {
        lockManager.releaseWriteLock(parent_inode_block);
        lockManager.releaseWriteLock(file_inode_block);
        throw std::runtime_error("Cannot delete non-empty directory\n");
    }

    //check that we own this entity before delete
    if (!request->isOwner(file_inode)) {
        lockManager.releaseWriteLock(parent_inode_block);
        lockManager.releaseWriteLock(file_inode_block);
        throw std::runtime_error("You do not own this directory\n");
    }

    removeDirentry(&parent_inode, parent_inode_block, direntry_idx, direntries, direntry_block);
    
    for (uint32_t i = 0; i < file_inode.size; i++) {
        blockManager.freeBlock(file_inode.blocks[i]);
    }

    lockManager.releaseWriteLock(file_inode_block);
    lockManager.deleteLock(file_inode_block); 

    blockManager.freeBlock(file_inode_block);

    lockManager.releaseWriteLock(parent_inode_block);
}

bool findEmptyDirentry(fs_inode *dir_inode, const char *filename, uint32_t *direntry_block, uint32_t *direntry_idx, fs_direntry *direntries) {
    uint32_t current_block = 0;
    bool found = false;
    fs_direntry direntry_buffer[FS_DIRENTRIES];

    for (uint32_t i = 0; i < dir_inode->size; i++) {
        current_block = dir_inode->blocks[i];
        disk_readblock(current_block, direntry_buffer);

        //Checks if there is a free direntry entry in current block, where
        //a free direntry is one where inode_block is 0
        for (uint32_t j = 0; j < FS_DIRENTRIES; j++) {
            if (!strcmp(direntry_buffer[j].name, filename)) {
                throw std::runtime_error("File/Directory already exists\n");
            }

            if (!found && !direntry_buffer[j].inode_block) {
                found = true;
                *direntry_block = current_block;
                *direntry_idx = j;
                memcpy(direntries, direntry_buffer, sizeof(fs_direntry) * FS_DIRENTRIES);
            }
        }
    }

    return found;
}

uint32_t findExistingDirentry(fs_inode *dir_inode, const char *filename, uint32_t *direntry_idx, uint32_t *direntry_block, fs_direntry *direntries) {
    //Checks for direntry in each data block of the directory
    for (uint32_t i = 0; i < dir_inode->size; i++) {
        *direntry_block = dir_inode->blocks[i];
        *direntry_idx = i;
        disk_readblock(*direntry_block, direntries);

        //Checks if the direntry is in the block in block_buffer
        for (uint32_t j = 0; j < FS_DIRENTRIES; j++) {

            //if this is the file
            if (!strcmp(direntries[j].name, filename)) {
                uint32_t freeBlock = direntries[j].inode_block; 
                direntries[j].inode_block = 0;
                memset(&direntries[j].name, 0, FS_MAXFILENAME + 1);

                return freeBlock;
            }
        }
    }

    throw std::runtime_error("File does not exist\n");
}

void removeDirentry(fs_inode *dir_inode, uint32_t dir_inode_block, uint32_t direntry_idx, fs_direntry *direntries, uint32_t direntry_block) {
    bool empty = true;

    //If the direntry block is now empty, it must be removed from the directory inode
    for (uint32_t i = 0; i < FS_DIRENTRIES; i++) {
        if (direntries[i].inode_block != 0) empty = false;
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

        blockManager.freeBlock(direntry_block);
    } else {
        disk_writeblock(direntry_block, direntries);
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
// NOTE: lock of the inode you want will be aquired when this returns
// 'write' should be true if you want to write lock, false if read lock
uint32_t traversePath(Request *request, const Path &path, int depth, bool write) {

    if(depth > path.depth()) {
        throw std::runtime_error("Invalid path\n");
    }

    uint32_t parentBlock = 0;
    uint32_t childBlock = 0;

    //write == true and depth == 0 implies that we want write permissions
    //for the root directory.  Otherwise we only need read permission
    if ((depth == 0) && write) {
        lockManager.aquireWriteLock(parentBlock);
    } else {
        lockManager.aquireReadLock(parentBlock);
    }

    for(int i = 0; i < depth; i++) {
        fs_inode parentDirectory;
        disk_readblock(parentBlock, &parentDirectory);

        //checks that we have permission to access this file
        if((parentBlock && !request->isOwner(parentDirectory))) {
            string message = string("\"")              +
                             path.getNameString(i - 1) +
                             string("\" permission denied.\n");
            lockManager.releaseReadLock(parentBlock);
            throw std::runtime_error(message);

        }

        //checks that we dont try to traverse into a file
        if (parentDirectory.type != 'd') {
            string message = string("\"")              +
                             path.getNameString(i - 1) +
                             string("\" is not a directory\n");
            lockManager.releaseReadLock(parentBlock);
            throw std::runtime_error(message);
        }

        const char *childName = path.getNameCString(i);
        childBlock = findBlock(&parentDirectory, childName);

        //findBlock returns 0 if it doesnt exist in directory 
        if (!childBlock) {
            string message = string("Directory \"") +
                             string(childName)      +
                             string("\" does not exist\n");
            lockManager.releaseReadLock(parentBlock);
            throw std::runtime_error(message);
        }

        //get lock for child
        //optionally gets the write lock if requested
        if ((i == depth - 1) && write) {
            lockManager.aquireWriteLock(childBlock);
        } else {
            lockManager.aquireReadLock(childBlock);
        }

        //release lock for parent
        lockManager.releaseReadLock(parentBlock);

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
