#ifndef LOCK_MANAGER_H
#define LOCK_MANAGER_H

#include <unordered_map>
#include <mutex>
#include "RWLock.h"

class LockManager {
public:
   void aquireReadLock(uint32_t block);
   void releaseReadLock(uint32_t block);

   void aquireWriteLock(uint32_t block);
   void releaseWriteLock(uint32_t block); 

   //This should be called when we create a new file
   //Call createLock with the file's inode
   void createLock(uint32_t block);

   //This should be called when we delete a file
   //Call deleteLock with the file's inode
   void deleteLock(uint32_t block);

private:
   std::unordered_map <uint32_t, RWLock> lockMap;
   std::mutex mapLock;
};

#endif
