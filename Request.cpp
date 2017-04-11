#include "Request.h"
#include "fs_crypt.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#include "fs_server.h"

using namespace std;

const int MAX_HEADER_SIZE = (sizeof(char) * FS_MAXUSERNAME) + sizeof(unsigned) + sizeof(char);


Request::Request(int in_sockfd) {
    sockfd = in_sockfd;
    data = nullptr;
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
            cerr << "error receiving message" << endl;
        }

        if (received == 0) return;

        if (header[i] == ' ') {
        	endOfUsername = i;
        	username = header;
        	username.pop_back();
            //strncpy(username, buffer, i); // copy username into buf
            //username[i] = '\0'; // strncpy does not null-terminate  
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
            cerr << "error receiving message" << endl;
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

    if (request == nullptr) close(sockfd);
    delete [] decrypted_msg_size;
}

void Request::parseRequestParameters() {
    request_type = parseRequestType();

    int i = 0;
    session = getNextInteger(i);
    sequence = getNextInteger(i);

    if (request_type == SESSION) return;

    path = new Path(request + i + 1);

    while (request[i] && request[i] != ' ') ++i; // TODO: Can there be a space in the path?

        if (request_type == READBLOCK || 
            request_type == WRITEBLOCK) 
        {
            block = atoi(request + i);

            if (request_type == WRITEBLOCK) 
            {
                while(request[i] && std::isdigit(request[i])) ++i; 
                ++i; // step over the <NULL>
                data = request + i;
            }	
        }
        else if (request_type == CREATE) {
            type = request[i + 1];
        }
}

int Request::getNextInteger(int &index) {
    while (!isdigit(request[index])) ++index;
    int res = atoi(request + index);
    while (isdigit(request[index])) ++index;
    return res;
}

REQUEST_T Request::parseRequestType() {
    char rt = request[3];
    if (rt == 'S')      return SESSION;
    else if (rt == 'R') return READBLOCK;
    else if (rt == 'W') return WRITEBLOCK;
    else if (rt == 'C') return CREATE;
    else return DELETE;
}

char Request::getType() {
    return type;
}

REQUEST_T Request::getRequestType() {
    return request_type;
}

Request::~Request() {
    delete [] request;
    delete path;
}
