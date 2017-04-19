#include "Request.h"
#include "fs_crypt.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <cassert>

#include "fs_server.h"

using namespace std;

const int MAX_HEADER_SIZE = (sizeof(char) * FS_MAXUSERNAME) + sizeof(unsigned) + sizeof(char);


Request::Request(int in_sockfd) {
    sockfd = in_sockfd;
    path = nullptr;
    data = nullptr;
    request = nullptr;
    isRead = false;
}

unsigned Request::getSession() { 
    return session; 
}

void Request::setSession(unsigned ses) { 
    session = ses; 
} 

unsigned Request::getSequence() { 
    return sequence; 
}

Path* Request::getPath() { 
    return path; 
}

std::string Request::getUsername() { 
    return username; 
}

unsigned Request::getBlock() {
    return block;
}

char* Request::getData() {
    return data;
}

void Request::sendResponse(char* text, unsigned size) {
    send(sockfd, text, size, 0);
}

void Request::parseHeader() {
    char header[MAX_HEADER_SIZE];
    int endOfUsername = 0;
    int i = 0;

    // receive 1 byte at a time until we get the whole header
    // receiving a ' ' denotes we have reached end of username
    // i - 1 because we're checking the most recent received byte
    while (i == 0 || header[i - 1] != '\0') {
        int received = recv(sockfd, header + i, 1, 0);

        if (received == -1) {
            close(sockfd);

            cout_lock.lock();
            cerr << "error receiving message" << endl;
            cout_lock.unlock();
        }

        if (received == 0) return;

        if (header[i] == ' ') {
        	endOfUsername = i;
        	username = header;
        	username.pop_back();
        } 
        i += received;
    }
    encrypted_request_size = atoi(header + endOfUsername);
}

void Request::parseRequestAndDecrypt(const char* password) {
    char* encrypted = new char[encrypted_request_size];
    int rcv = 0;
    while (rcv < encrypted_request_size) {
        int received = recv(sockfd, encrypted + rcv, encrypted_request_size - rcv, 0);
        if (received == -1) {
            close(sockfd);

            cout_lock.lock();
            cerr << "error receiving message" << endl;
            cout_lock.unlock();
        }
        if (received == 0) break;
        rcv += received;
    }
    decryptRequest(password, encrypted);
    delete [] encrypted;
}

void Request::decryptRequest(const char* password, char* encrypted) {
    unsigned int* decrypted_msg_size = new unsigned int[1];
    
    request = static_cast<char*>(fs_decrypt(password, encrypted, encrypted_request_size, decrypted_msg_size));
    request_size = decrypted_msg_size[0];
    
    delete [] decrypted_msg_size;
 
    if (request == nullptr) {
        throw std::runtime_error("can't decrypt");
    }
}

void Request::parseRequestParameters() {
    request_type = parseRequestType();

    int i = 0;
    session = getNextInteger(i);
    sequence = getNextInteger(i);

    if (request_type == SESSION) return;

    string pathString = getPathString(i);
    try {
        path = new Path(pathString.c_str());
    } catch (std::runtime_error &e) {
        throw e;
    }

    if (request_type == READBLOCK || 
        request_type == WRITEBLOCK) 
    {
        block = getNextInteger(i); 

        if (request_type == WRITEBLOCK) 
        {
            memcpy(data, request + i, FS_BLOCKSIZE);
        }	
    }
    else if (request_type == CREATE) {
        type = request[i];
        assert(type == 'f' || type == 'd');
    }
}

int Request::getNextInteger(int &index) {
    while (!isdigit(request[index])) ++index;
    int res = atoi(request + index);
    while (isdigit(request[index])) ++index;
    ++index;
    return res;
}

//I made this to provide the same functionality as 
//getNextInteger except with the path name. It takes
//the index of the beginning of the path and returns
//a string containing the path. When the function returns,
//index is the index of the character just after the path
//delimiter (' ' or '\n')
string Request::getPathString(int &index) {
    uint32_t i;
    char path[FS_MAXPATHNAME + 1];

    for (i = 0; request[index] != ' ' && request[index] != '\0'; i++, index++) {
        if (i >= FS_MAXPATHNAME) {
            throw std::runtime_error("Path exceeds maximum length\n");
        }
        path[i] = request[index];
    }

    assert(request[index] == ' ' || request[index] == '\0');
    path[i] = '\0';
    index++;
    return string(path);
}

REQUEST_T Request::parseRequestType() {
    char rt = request[3];
    REQUEST_T returnVal;

    if (rt == 'S') {
        returnVal = SESSION;

    } else if (rt == 'R') {
        isRead = true;
        returnVal =  READBLOCK;
        initializeData();
    } else if (rt == 'W') {
        returnVal = WRITEBLOCK;
        initializeData();
    } else if (rt == 'C') {
        returnVal = CREATE;

    } else {
        returnVal = DELETE;
    }

    return returnVal;
}

char Request::getType() {
    return type;
}

REQUEST_T Request::getRequestType() {
    return request_type;
}

void Request::initializeData() {
    data = new char[FS_BLOCKSIZE];
}

bool Request::isReadRequest() {
    return isRead;
}

void Request::validateInput() {
    if (request_type != SESSION) {
        try {
            sessionManager.validateRequest(session, sequence, username.c_str());
        } catch (std::runtime_error &e) {
            throw e;
        }
    }
}

Request::~Request() {
    delete [] request;
    delete path;
    delete[] data;
}

bool Request::isOwner(fs_inode &inode) {
    return !strcmp(inode.owner, this->getUsername().c_str());
}
