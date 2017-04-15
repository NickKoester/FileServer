#include "LockManager.h"
using namespace std;

void LockManager::aquireReadLock(uint32_t block) {
    mapLock.lock();

    lockMap[block].readerStart();

    mapLock.unlock();
}

void LockManager::releaseReadLock(uint32_t block) {
    mapLock.lock();

    lockMap[block].readerFinish();

    mapLock.unlock();
}

void LockManager::aquireWriteLock(uint32_t block) {
    mapLock.lock();

    lockMap[block].writerStart();

    mapLock.unlock();
}

void LockManager::releaseWriteLock(uint32_t block) {
    mapLock.lock();

    lockMap[block].writerFinish();

    mapLock.unlock();
}

void LockManager::createLock(uint32_t block) {
    mapLock.lock();

    lockMap[block];

    mapLock.unlock();
}

void LockManager::deleteLock(uint32_t block) {
    mapLock.lock();

    lockMap.erase(block);

    mapLock.unlock();
}
