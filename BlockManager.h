#include <queue>

class BlockManager {
public:
    void initialize();
    uint32_t getFreeBlock();
private:
    std::queue<uint32_t> freeBlocks;
};
