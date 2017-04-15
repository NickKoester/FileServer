#include "LockManager.h"
using namespace std;

void LockManager::aquireReadLock(uint32_t block) {
    lockMap[block].readerStart();
}

void LockManager::releaseReadLock(uint32_t block) {
    lockMap[block].readerFinish();
}

void LockManager::aquireWriteLock(uint32_t block) {
    lockMap[block].writerStart();
}

void LockManager::releaseWriteLock(uint32_t block) {
    lockMap[block].writerFinish();
}

void LockManager::createLock(uint32_t block) {
    lockMap[block];
}

void LockManager::deleteLock(uint32_t block) {
    lockMap.erase(block);
}
