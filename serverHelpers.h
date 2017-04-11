#ifndef __SERVERHELPERS_H__
#define __SERVERHELPERS_H__

#include <unordered_map>
#include "fs_server.h"
#include "Path.h"
#include "Request.h"


extern std::unordered_map<std::string, std::string> users;


void requestHandler(int sockfd);

void initializeUsers(std::unordered_map<std::string, std::string> &users);

int getIncomingRequestSize(const char*);

void processHeader(int sockfd, char* buf, int& message_size);

void processRequest(int sockfd, char* buf, int expected);

unsigned int getSessionNumber(char* msg);

unsigned int getSequenceNumber(char* msg);

Path getPathname(char *msg);

unsigned int getBlockNum(char *msg);

char getType(char *msg);

char *getData(char *msg);

REQUEST_T getRequestType(char* rq);

int createCleartextHeader(char* buf, unsigned int s);

char* encryptResponse(const char* password, char* plaintext, int plaintext_size, unsigned int * encrypted_size);

#endif
