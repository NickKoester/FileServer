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
    memset(username, 0, FS_MAXUSERNAME + 1);
    
    int msg_size = 0; 
    processHeader(sockfd, username, msg_size);
       
    char* msg = new char[msg_size]; //TODO: do we need msg_size+1???
    processRequest(sockfd, msg, msg_size);

    string user(username);
    unsigned int* decrypted_msg_size = new unsigned int[1];
    char* decrypted = static_cast<char*>(fs_decrypt(users[user].c_str(), msg, msg_size, decrypted_msg_size));

    if (decrypted == nullptr) close(sockfd);


    REQUEST_T    requestType = getRequestType(decrypted);
    unsigned int sessionNum  = getSessionNumber(decrypted);
    unsigned int sequenceNum = getSequenceNumber(decrypted);
    unsigned int blockNum    = getBlockNum(decrypted);

    Path path = getPathname(decrypted);

    // the return data for READBLOCK
    char *data = nullptr;

    switch(requestType)
    {
        case SESSION:
            cout << "Session Request\n sequence number: " << sequenceNum << '\n';
            sessionNum = sessionRequest(sequenceNum, username); 
            break;

        case READBLOCK:
            cout << "Readblock Request\n";
            data = readRequest(username, path, blockNum);
            // NEED TO FREE DATA 
            break;

        case WRITEBLOCK:
            cout << "Writeblock Request\n";
            writeRequest(username, path, blockNum, getData(decrypted));
            break;

        case CREATE:
            cout << "Create Request\n";
            createRequest(username, path, getType(decrypted));
            break;

        case DELETE:
            cout << "Delete Request\n";
            deleteRequest(username, path);
            break;

        default:
            cout << "Fuck\n";
            break;
    }
        
    //char* res = fs_encrypt(users[testuser].c_str(), uncrypt_res, uncrypt_size, sizet);
    unsigned response_size = 0;
    char* res = createResponse(sessionNum, sequenceNum, data, response_size); 

    unsigned int* encrypted_size = new unsigned int[1];
    char* encrypted_response = fs_encrypt(users[user].c_str(), res, response_size, encrypted_size);

    send(sockfd, encrypted_response, encrypted_size, 0);

        
    delete [] decrypted;
    delete [] msg;
    delete [] decrypted_msg_size;
    delete [] encrypted_size;
    delete [] res;
    close(sockfd);
}

/* Initializes the username/password map */
void initializeUsers(std::unordered_map<std::string, std::string> &users) {
    string user;
    string password;
    while (cin >> user) {
        cin >> password;
        users[user] = password;
    }
}

/* Returns the size of the incoming encrypted request */
int getIncomingRequestSize(const char *buff) {
    int i = 0;
    while(buff[i++] != ' ');
    int length = atoi(buff + i);
    return length;
}

/* Copies username into buf. Sets message_size equal to the size of the incoming request */
void processHeader(int sockfd, char* buf, int& message_size) {
    char buffer[MAX_HEADER_SIZE];
    int i = 0;
    int size_of_username = 0;

    // receive 1 byte at a time until we get the whole header
    // receiving a ' ' denotes we have reached end of username
    // i - 1 because we're checking the most recent received byte
    while (i == 0 || buffer[i - 1] != '\0') {
        int received = recv(sockfd, buffer + i, 1, 0);
        if (received == -1) {
            close(sockfd);
            cerr << "error receiving message" << endl;
        }
        if (received == 0) return;
        if (buffer[i] == ' ') {
            strncpy(buf, buffer, i); // copy username into buf
            buf[i] = '\0'; // strncpy does not null-terminate  
        }
        i += received;
    }
    message_size = getIncomingRequestSize(buffer);
} 

/* Copies the received encrypted request into buf */
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

unsigned int getSessionNumber(char* msg) {
    int i = 0;
    while(!isdigit(msg[i++ + 1]));
    return atoi(msg + i);
}

unsigned int getSequenceNumber(char* msg) {
    int i = 0;
    while (!isdigit(msg[i])) ++i;
    return atoi(msg + i + 1);
}

Path getPathname(char *msg) {
    int spaceCount = 0;
    int i = 0;
   
    //pathname is right after the third space
    while(msg[i] && spaceCount != 3) {
        if (msg[i] == ' ') {
            ++spaceCount;
        }
    }

    //the assumption here is that we dont really care about
    //what Path is if we give it a nullptr
    //Also assuming '\0' comes right after filename
    //I will have to think about what to make of this
    return Path(msg + i);
}

unsigned int getBlockNum(char *msg) {
    int spaceCount = 0;
    int i = 0;
   
    //block is right after the fourth space
    while(msg[i] && spaceCount != 4) {
        if (msg[i] == ' ') {
            ++spaceCount;
        }
        ++i;
    }

    return atoi(msg + i);
}

char getType(char *msg) {
    int spaceCount = 0;
    int i = 0;
   
    //type is right after the fourth space
    while(msg[i] && spaceCount != 4) {
        if (msg[i] == ' ') {
            ++spaceCount;
        }
        ++i;
    }

    return msg[i];
}

char *getData(char *msg) {
    int i = 0;
    while(msg[i++]);
    return msg + i;
}

REQUEST_T getRequestType(char* rq) {
    char type = rq[3];
    if (type == 'S') return SESSION;
    else if (type == 'R') return READBLOCK;
    else if (type == 'W') return WRITEBLOCK;
    else if (type == 'C') return CREATE;
    else return DELETE;
}

