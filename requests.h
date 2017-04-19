#ifndef REQUESTS_H
#define REQUESTS_H

#include "Path.h"
#include "Request.h"

/* This file contains the main functions that we use to modify our
 * file system.  Most of the hand over hand locking is done in the 
 * traversePath function.  Most of the code in these functions are 
 * dedicated to error checking.
 */

//Function that does a session request
//Doesnt need locks
void sessionRequest(Request *request);

//This does a create request.
//This gets a write lock on the directory
//and creates a new lock for the new file
void createRequest(Request *request);

//Write request function
//Gets write lock on the file being written
void writeRequest(Request *request);

//Read request function
//Gets read lock on file
//Shouldnt change anything
void readRequest(Request *request);

//Delete request function
//Gets write lock on directory that the deleted
//file is in.  Also deletes the lock for the file.
void deleteRequest(Request *request);

//This function creates the response to send back to client
//This function should probably be somewhere else but
//we never got around to moving it
char *createResponse(Request *request, unsigned &response_size);

//Im not even sure why this is here
char *getName(const char *pathname);

//I dont think we use this
fs_inode *getParentInode(const char *pathname);

//I dont think we use this either
fs_direntry *findEmptyDirentry(fs_inode *inode);

//Finds an empty direntry in dir_inode. If found it assigns direntry_block
//to the block that it was in, and it assigns direntry_idx to the index in
//the array.  It copies the block into direntries.  Returns false if no empty
//direntry was found.
bool findEmptyDirentry(fs_inode *dir_inode, const char *filename, uint32_t *direntry_block, uint32_t *direntry_idx, fs_direntry *direntries);

//Finds the location of the direntry that contains filename.  If found it fills
//direntry_idx and direntry_block with the relevent information.  It copies the
//direntry block into direntries.  Returns the block that the direntry points to.
uint32_t findExistingDirentry(fs_inode *dir_inode, const char *filename, uint32_t *direntry_idx, uint32_t *direntry_block, fs_direntry *direntries);

//Updates the direntry array when we want to remove a file.
//We pass in direntry block and the index of the thing we 
//want to remove.
void removeDirentry(fs_inode *dir_inode, uint32_t dir_inode_block, uint32_t direntry_idx, fs_direntry *direntries, uint32_t direntry_block);

//This function looks for the direntry named 'name' in the inode
//given to it and it returns the block where the found inode
//is located
uint32_t findBlock(fs_inode *parent, const char *name);

//Takes the path and returns the block that contains the entity
//specified by depth.  Takes the Request object just to verify
//that the entity is owned by the user who made the request.
//This function is the one responsible for most of the hand over
//hand locking.  'write' should be true if we wish to obtain the
//write lock on the entity we are traversing to, otherwise it will
//return with a read lock.
//This function returns with a lock on the block that is returned. 
uint32_t traversePath(Request *request, const Path &path, int depth, bool write);

//Returns true if the username in the request
//owns the inode
bool isOwner(fs_inode*, Request*);

#endif
