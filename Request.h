#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "Path.h"

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

    std::string getUsername();

    void sendResponse(char* text, unsigned size);

    REQUEST_T getRequestType();
 
    char getType();

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

    void decryptRequest(const char* password, char* encrypted);

    REQUEST_T parseRequestType();

    int getNextInteger(int &index);
};

#endif    
