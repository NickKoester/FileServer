#ifndef BLOCK_MANAGER_H
#define BLOCK_MANAGER_H

#include <queue>
#include <mutex>

//This class is responsible managing the free blocks in our system
//We assume that any block on the queue is free
class BlockManager {
public:
    //This function is called once when the file server starts
    //It traverses the file system and puts all the free blocks
    //on the queue
    void initialize();

    //Called when a request needs a new block
    //Removes a block from the queue and
    //returns it
    uint32_t getFreeBlock();

    //This is called when we are freeing a block
    //We put it on the queue and assume its available
    //to use
    void freeBlock(uint32_t block);
private:
    std::queue<uint32_t> freeBlocks;
    std::mutex blockLock;
};

#endif
