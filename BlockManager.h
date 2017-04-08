#include <queue>

class BlockManager {
public:
    void initialize();
    uint32_t getFreeBlock();
    void freeBlock(uint32_t block);
private:
    std::queue<uint32_t> freeBlocks;
};
