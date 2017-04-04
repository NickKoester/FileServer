#include <mutex>
#include <condition_variable>

class RWLock {
public:
    readerStart();
    readerFinish();

    writerStart();
    writerFinish();
private:
    std::mutex rwLock;
    std::condition_variable waitingReaders;
    std::condition_variable waitingWriters;

    size_t numReaders;
    size_t numWriters;
    size_t waitingWriters;
};
