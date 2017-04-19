#ifndef RWLOCK_H
#define RWLOCK_H

#include <mutex>
#include <condition_variable>

//Reader/Writer locks that we use on each entity of the file system.
//These locks allow multiple readers concurrently but only
//one writer at a time. One RWLock is given to each file system entity
//and is stored in global LockManager. 
class RWLock {
public:
    //Interface for readers of file
    void readerStart();
    void readerFinish();

    //Interface for writers of file
    void writerStart();
    void writerFinish();
private:
    std::mutex rwLock;
    std::condition_variable_any cvReaders;
    std::condition_variable_any cvWriters;

    size_t numReaders;
    size_t numWriters;
    size_t waitingWriters;
};

#endif
