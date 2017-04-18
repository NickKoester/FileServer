#include <vector>
#include "fs_server.h"
#include "BlockManager.h"
#include "LockManager.h"

using namespace std;

extern LockManager lockManager;

void BlockManager::initialize() {
    blockLock.lock();

    //keep track of whats free
    vector<bool> isFree(FS_DISKSIZE, true);

    //keep track of valid directory blocks (0 is '/' directory)
    queue <uint32_t> traverseQueue;
    traverseQueue.push(0);

    while (!traverseQueue.empty()) {
        fs_inode currentNode;

        uint32_t blocknum = traverseQueue.front();
        traverseQueue.pop();

        lockManager.createLock(blocknum);

        isFree[blocknum] = false;

        lockManager.aquireReadLock(blocknum);

        //read inode of currentNode
        disk_readblock(blocknum, &currentNode);

        for (uint32_t i = 0; i < currentNode.size; i++) {
            isFree[currentNode.blocks[i]] = false;

            //if its a file theres no more work to do
            if(currentNode.type == 'f') {
                continue;
            }

            //if its a directory we get all other inodes
            fs_direntry direntries[FS_DIRENTRIES];
            disk_readblock(currentNode.blocks[i], direntries);

            for(unsigned int j = 0; j < FS_DIRENTRIES; j++) {
                uint32_t inode_block = direntries[j].inode_block;

                if(inode_block != 0) {
                    //pushes inode blocks onto the queue
                    traverseQueue.push(inode_block);
                }
            }
        }

        lockManager.releaseReadLock(blocknum);
    }

    for(size_t i = 0; i < isFree.size(); i++) {
        if(isFree[i]) {
            freeBlocks.push(i);
        }
    }

   blockLock.unlock();
}

uint32_t BlockManager::getFreeBlock() {
    blockLock.lock();

    uint32_t freeBlock = freeBlocks.front();
    freeBlocks.pop();

    blockLock.unlock();
    return freeBlock;
}

void BlockManager::freeBlock(uint32_t block) {
    blockLock.lock();

    freeBlocks.push(block);

    blockLock.unlock();
}
