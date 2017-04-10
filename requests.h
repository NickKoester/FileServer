#ifndef REQUESTS_H
#define REQUESTS_H

#include "Path.h"
#define DIRENTRIES_PER_BLOCK FS_BLOCKSIZE / sizeof(fs_direntry)

unsigned int sessionRequest(unsigned int seq, const char *username);
void createRequest(const char *username, const Path &path, char type);
void writeRequest(const char* username, const Path &path, unsigned int block, const char* data);
char *readRequest(const char* username, const Path &path, unsigned int block);
char *createResponse(unsigned int sesh, unsigned int seq, const char *, unsigned &response_size);
void deleteRequest(const char * username, const Path &path);
char *getName(const char *pathname);
fs_inode *getParentInode(const char *pathname);
fs_direntry *findEmptyDirentry(fs_inode *inode);
void addDirentry(fs_inode *dir_inode, const uint32_t dir_inode_block, const char * file_name, const uint32_t file_inode_block);
void removeDirentry(fs_inode *dir_inode, const uint32_t dir_inode_block, const uint32_t file_inode_block);
uint32_t findBlock(fs_inode *parent, const char *name);
uint32_t traversePath(const Path &path, int depth);

#endif
