#ifndef LOCK_MANAGER_H
#define LOCK_MANAGER_H

#include <unordered_map>
#include <mutex>
#include "RWLock.h"

//We use this class to keep track of the locks
//for each entity in our file system
//The functions provide access to the locks
//We assume functions are only called on blocks
//that contain inode entities
class LockManager {
public:
   //I think these functions are self explainitory
   //Should only be called on inode blocks
   void aquireReadLock(uint32_t block);
   void releaseReadLock(uint32_t block);

   //Same deal as above
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
