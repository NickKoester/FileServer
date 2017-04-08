#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <unordered_map>

#include "fs_server.h"
#include "fs_crypt.h"

#include "requests.h"
#include "BlockManager.h"
#include "SessionManager.h"
using namespace std;

enum REQUEST_T { SESSION, READBLOCK, WRITEBLOCK, CREATE, DELETE };

struct sockaddr_in addr, cli_addr;

unordered_map<string, string> users;

const int MAX_HEADER_SIZE = (sizeof(char) * FS_MAXUSERNAME) + sizeof(unsigned) + sizeof(char);

int getMessageLength(const char*);

void processHeader(int sockfd, char* buf, int& message_size);

void processRequest(int sockfd, char* buf, int expected);

unsigned int getSequenceNumber(char* msg);

REQUEST_T getRequestType(char* rq);

BlockManager blockManager;
SessionManager sessionManager;

int main(int argc, char *argv[])
{
/*  1. Read the list of usernames and passwords from stdin
 *  2. Initialize the list of free disk blocks
 *  3. Set up the socket and call listen.
 */

/*
 *  TODO: split this into more functions
 *  TODO: change strings to c_strs?
 *
 */


//  1)
    string user;
    string password;
    while (cin >> user) {
        cin >> password;
        users[user] = password;
    }

//  2)
    blockManager.initialize();

//  3)
    /******* SOCKET STUFF *********/
    int port_number = 0;
    if(argc > 1) {
        port_number = stoi(argv[1]);
    }

    // Creating a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        cerr << "Error opening stream socket\n";
        return -1;
    }
    
    memset(&addr, 0, sizeof(addr));

    int enable = 1;
    // Reusing ports or something
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
        cerr << "Error in setsockopt\n";
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_number);
    
    if (bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        cerr << "Error binding stream socket\n";
        return -1;
    }

    socklen_t length = sizeof(addr);
    if (getsockname(sockfd, (struct sockaddr *) &addr, &length) == -1) {
        cerr << "Error getting socket name\n";
        return -1;
    }

    cout << "\n@@@ port " << ntohs(addr.sin_port) << endl;

    listen(sockfd, 10);

    /********* SERVER STUFF *******/
    while (1) {
        int msg_fd = accept(sockfd, nullptr, nullptr);

     //TODO -- Can we move everything below this point into a new function?
     //        Check out requestStub.cpp

        char username[FS_MAXUSERNAME + 1]; // TODO: dynamic size? +1 for null
        int msg_size = 0; 
        memset(username, 0, FS_MAXUSERNAME + 1);
        
        processHeader(msg_fd, username, msg_size);
       
        char* msg = new char[msg_size]; //TODO: do we need msg_size+1???
        processRequest(msg_fd, msg, msg_size);

        string testuser(username);
        unsigned int* sizet = new unsigned int[1];
        char* decryptd = static_cast<char*>(fs_decrypt(users[testuser].c_str(), msg, msg_size, sizet));

        if (decryptd == nullptr) close(msg_fd);


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
        close(msg_fd);
    }
       
    return 0;
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

REQUEST_T getRequestType(char* rq) {
    char type = rq[3];
    if (type == 'S') return SESSION;
    else if (type == 'R') return READBLOCK;
    else if (type == 'W') return WRITEBLOCK;
    else if (type == 'C') return CREATE;
    else return DELETE;
}

unsigned int getSequenceNumber(char* msg) {
    int i = 0;
    while (!isdigit(msg[i])) ++i;
    return atoi(msg + i + 1);
}

