#ifndef REQUESTS_H
#define REQUESTS_H

#include "Path.h"
#include "Request.h"

void sessionRequest(Request *request);

void createRequest(Request *request);

void writeRequest(Request *request);

void readRequest(Request *request);

void deleteRequest(Request *request);

char *createResponse(Request *request, unsigned &response_size);

char *getName(const char *pathname);
fs_inode *getParentInode(const char *pathname);
fs_direntry *findEmptyDirentry(fs_inode *inode);
bool findEmptyDirentry(fs_inode *dir_inode, const char *filename, uint32_t *direntry_block, uint32_t *direntry_idx, fs_direntry *direntries);
uint32_t findExistingDirentry(fs_inode *dir_inode, const char *filename, uint32_t *direntry_idx, uint32_t *direntry_block, fs_direntry *direntries);
void removeDirentry(fs_inode *dir_inode, uint32_t dir_inode_block, uint32_t direntry_idx, fs_direntry *direntries, uint32_t direntry_block);
uint32_t findBlock(fs_inode *parent, const char *name);
uint32_t traversePath(Request *request, const Path &path, int depth, bool write);

bool isOwner(fs_inode*, Request*);

#endif
