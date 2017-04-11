#ifndef REQUESTS_H
#define REQUESTS_H

#include "Path.h"
#include "Request.h"

void sessionRequest(Request *request);

void createRequest(Request *request);

void writeRequest(Request *request);

char *readRequest(Request *request);

void deleteRequest(Request *request);

char *createResponse(Request *request, unsigned &response_size);

char *getName(const char *pathname);
fs_inode *getParentInode(const char *pathname);
fs_direntry *findEmptyDirentry(fs_inode *inode);
void addDirentry(fs_inode *dir_inode, const uint32_t dir_inode_block, const char * file_name, const uint32_t file_inode_block);
void removeDirentry(fs_inode *dir_inode, const uint32_t dir_inode_block, const uint32_t file_inode_block);
uint32_t findBlock(fs_inode *parent, const char *name);
uint32_t traversePath(const Path &path, int depth);

#endif
