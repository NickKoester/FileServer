#include <queue>
#include "fs_server.h"
using namespace std;

queue<uint32_t> freeBlocks;

void initializeDisk() {
    fs_inode *rootDir;
    disk_readblock(0, rootDir);

    fs_direntry

    while(
    freeBlocks.push(rootDir->blocks[i]);
