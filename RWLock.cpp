#include "RWLock.h"
using namespace std;

void RWLock::readerStart() {
    rwLock.lock();

    while (numWriters + waitingWriters > 0) {
        waitingReaders.wait();
    }

    numReaders++;

    rwLock.unlock();
}

void RWLock::readerFinish() {
    rwLock.lock();

    numReaders--;

    if (numReaders == 0) {
        waitingWriters.notify_one();
    }

    rwLock.unlock();
}

void RWLock::writerStart() {
    rwLock.lock();

    waitingWriters++;

    while (numReaders + numWriters > 0) {
        waitingWriters.wait();
    }

    waitingWriters--;
    numWriters++;

    rwLock.unlock();
}

void RWLock::writerFinish() {
    rwLock.lock();
    numWriters--;
    waitingReaders.broadcast();
    waitingWriters.signal();

    rwLock.unlock();
}
