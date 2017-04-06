#include <queue>

class BlockManager {
public:
    void initialize();
private:
    std::queue<uint32_t> freeBlocks;
};
