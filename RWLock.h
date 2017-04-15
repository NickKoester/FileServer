#ifndef RWLOCK_H
#define RWLOCK_H

#include <mutex>
#include <condition_variable>

class RWLock {
public:
    void readerStart();
    void readerFinish();

    void writerStart();
    void writerFinish();
private:
    std::unique_lock<std::mutex> rwLock;
    std::condition_variable cvReaders;
    std::condition_variable cvWriters;

    size_t numReaders;
    size_t numWriters;
    size_t waitingWriters;
};

#endif
