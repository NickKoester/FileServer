#ifndef BLOCK_MANAGER_H
#define BLOCK_MANAGER_H

#include <queue>
#include <mutex>

class BlockManager {
public:
    void initialize();
    uint32_t getFreeBlock();
    void freeBlock(uint32_t block);
private:
    std::queue<uint32_t> freeBlocks;
    std::mutex blockLock;
};

#endif
