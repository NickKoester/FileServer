#include "serverHelpers.h"
#include "requests.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <string>
using namespace std;

void requestHandler(int sockfd) {

    char username[FS_MAXUSERNAME + 1]; // TODO: dynamic size? +1 for null
    int msg_size = 0; 
    memset(username, 0, FS_MAXUSERNAME + 1);
        
    processHeader(sockfd, username, msg_size);
       
    char* msg = new char[msg_size]; //TODO: do we need msg_size+1???
    processRequest(sockfd, msg, msg_size);

    string testuser(username);
    unsigned int* sizet = new unsigned int[1];
    char* decryptd = static_cast<char*>(fs_decrypt(users[testuser].c_str(), msg, msg_size, sizet));

    if (decryptd == nullptr) close(sockfd);


    REQUEST_T requestType = getRequestType(decryptd);
    //unsigned session = 0;   Andrew what is this for?
    switch(requestType)
    {
        case SESSION:
            cout << "Session Request\n sequence number: " << getSequenceNumber(decryptd);
            sessionRequest(getSequenceNumber(decryptd), username); 
            break;
        case READBLOCK:
            cout << "Readblock Request\n";
            break;
        case WRITEBLOCK:
            cout << "Writeblock Request\n";
            break;
        case CREATE:
            cout << "Create Request\n";
            break;
        case DELETE:
            cout << "Delete Request\n";
            break;
        default:
            cout << "Fuck\n";
            break;
        }
        
        //char* res = fs_encrypt(users[testuser].c_str(), uncrypt_res, uncrypt_size, sizet);
        
    delete [] decryptd;
    delete [] msg;
    close(sockfd);
}

void initializeUsers(std::unordered_map<std::string, std::string> &users) {
    string user;
    string password;
    while (cin >> user) {
        cin >> password;
        users[user] = password;
    }
}


int getMessageLength(const char *buff) {
    int i = 0;
    while(buff[i++] != ' ');
    int length = atoi(buff + i);
    return length;
}

void processHeader(int sockfd, char* buf, int& message_size) {
    char buffer[MAX_HEADER_SIZE];
    int i = 0;
    int user_size = 0;
    // i - 1 because we're checking the last received byte
    while (i == 0 || buffer[i - 1] != '\0') {
        int received = recv(sockfd, buffer + i, 1, 0);
        if (received == -1) {
            close(sockfd);
            cerr << "error receiving message" << endl;
        }
        if (received == 0) return;
        if (buffer[i] == ' ') 
            user_size = i;
        i += received;
    }
    strncpy(buf, buffer, user_size); 
    buf[user_size] = '\0'; // strncpy does not null-terminate
    message_size = getMessageLength(buffer);
} 


void processRequest(int sockfd, char* buf, int expected) {
    int rcv = 0;
    while (rcv < expected) {
        int received = recv(sockfd, buf + rcv, expected - rcv, 0);
        if (received == -1) {
            close(sockfd);
            cerr << "error receiving message" << endl;
        }
        if (received == 0) break;
        rcv += received;
    }
}

unsigned int getSequenceNumber(char* msg) {
    int i = 0;
    while (!isdigit(msg[i])) ++i;
    return atoi(msg + i + 1);
}

REQUEST_T getRequestType(char* rq) {
    char type = rq[3];
    if (type == 'S') return SESSION;
    else if (type == 'R') return READBLOCK;
    else if (type == 'W') return WRITEBLOCK;
    else if (type == 'C') return CREATE;
    else return DELETE;
}

