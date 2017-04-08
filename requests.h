#ifndef REQUESTS_H
#define REQUESTS_H

#include "Path.h"

unsigned int sessionRequest(unsigned int seq, const char *username);
void createRequest(unsigned int sesh, unsigned int seq, const char *pathname, const char *username, char type);
void sendResponse(unsigned int sesh, unsigned int seq);
char *getName(const char *pathname);
fs_inode *getParentInode(const char *pathname);
fs_direntry *findEmptyDirentry(fs_inode *inode);
uint32_t findBlock(fs_inode *parent, const char *name);
uint32_t traversePath(const Path &path, int depth);

#endif
