#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <string>
#include "SessionManager.h"
#include "fs_server.h"
#include "Path.h"

extern SessionManager sessionManager;
enum REQUEST_T { SESSION, READBLOCK, WRITEBLOCK, CREATE, DELETE };

class Request {
public:
    Request(int in_sockfd);

    void parseHeader();

    void parseRequestAndDecrypt(const char* password);

    void parseRequestParameters();

    unsigned getSession();

    void setSession(unsigned ses);

    unsigned getSequence();

    Path* getPath();

    unsigned getBlock();

    char* getData();

    void initializeData();

    std::string getUsername();

    void sendResponse(char* text, unsigned size);

    REQUEST_T getRequestType();
 
    char getType();

    bool isReadRequest();

    void validateInput();

    bool isOwner(fs_inode &inode);

    ~Request();

private:
    int sockfd;
    std::string username;

    char* request;
    int request_size;
    int encrypted_request_size;

    Path* path;
    REQUEST_T request_type;
    unsigned session;
    unsigned sequence;
    unsigned block;
    char type;
    char* data;
    bool isRead;

    void decryptRequest(const char* password, char* encrypted);

    REQUEST_T parseRequestType();

    int getNextInteger(int &index);

    std::string getPathString(int &index);
};

#endif    
