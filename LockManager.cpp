#include "LockManager.h"
using namespace std;

void LockManager::aquireReadLock(uint32_t block) {
    mapLock.lock();

    RWLock *rwlock = &lockMap[block];

    mapLock.unlock();

    rwlock->readerStart();
}

void LockManager::releaseReadLock(uint32_t block) {
    mapLock.lock();

    RWLock *rwlock = &lockMap[block];

    mapLock.unlock();

    rwlock->readerFinish();
}

void LockManager::aquireWriteLock(uint32_t block) {
    mapLock.lock();

    RWLock *rwlock = &lockMap[block];

    mapLock.unlock();

    rwlock->writerStart();
}

void LockManager::releaseWriteLock(uint32_t block) {
    mapLock.lock();

    RWLock *rwlock = &lockMap[block];

    mapLock.unlock();

    rwlock->writerFinish();
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
