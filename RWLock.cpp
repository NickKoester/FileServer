#include "RWLock.h"
using namespace std;

void RWLock::readerStart() {
    rwLock.lock();

    while (numWriters + waitingWriters > 0) {
        cvReaders.wait(rwLock);
    }

    numReaders++;

    rwLock.unlock();
}

void RWLock::readerFinish() {
    rwLock.lock();

    numReaders--;

    if (numReaders == 0) {
        cvWriters.notify_one();
    }

    rwLock.unlock();
}

void RWLock::writerStart() {
    rwLock.lock();

    waitingWriters++;

    while (numReaders + numWriters > 0) {
        cvWriters.wait(rwLock);
    }

    waitingWriters--;
    numWriters++;

    rwLock.unlock();
}

void RWLock::writerFinish() {
    rwLock.lock();
    numWriters--;
    cvReaders.notify_all();
    cvWriters.notify_one();

    rwLock.unlock();
}
