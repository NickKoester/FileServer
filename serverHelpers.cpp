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

    Request request(sockfd);

    request.parseHeader();

    // very important that this is not before request.parseHeader();
    const char* password = users[request.getUsername()].c_str(); 
    request.parseRequestAndDecrypt(password);
   
    try {
        request.parseRequestParameters();
        request.validateInput();
    } catch (std::runtime_error &e) {
        cerr << e.what();
        close(sockfd);
        return;
    }

    switch(request.getRequestType())
    {
        case SESSION:
            cout << "Session Request\n";

            try {
                sessionRequest(&request); 
            } catch (std::runtime_error &e) {
                cerr << e.what();
                close(sockfd);
                return;
            }
            break;

        case READBLOCK:
            cout << "Readblock Request\n";

            try {
                readRequest(&request);
            } catch (std::runtime_error &e) {
                cerr << e.what();
                close(sockfd);
                return;
            }
            break;

        case WRITEBLOCK:
            cout << "Writeblock Request\n";

            try {
                writeRequest(&request);
            } catch (std::runtime_error &e) {
                cerr << e.what();
                close(sockfd);
                return;
            }
            break;

        case CREATE:
            cout << "Create Request\n";

            try {
                createRequest(&request);
            } catch (std::runtime_error &e) {
                cerr << e.what();
                close(sockfd);
                return;
            }
            break;

        case DELETE:
            cout << "Delete Request\n";

            try {
                deleteRequest(&request);
            } catch (std::runtime_error &e) {
                cerr << e.what();
                close(sockfd);
                return;
            }
            break;

        default:
            cerr << "Invalid request\n";
            close(sockfd);
            return;
    }
        
    unsigned response_size = 0;
    char* res = createResponse(&request, response_size); 

    unsigned int* encrypted_size = new unsigned int[1];
    char* encrypted_response = encryptResponse(password, res, response_size, encrypted_size);

    char cleartext[11];
    int cleartextSize = createCleartextHeader(cleartext, *encrypted_size);

    // CLEARTEXT HEADER
    request.sendResponse(cleartext, cleartextSize);

    // ENCRYPTED RESPONSE
    request.sendResponse(encrypted_response, *encrypted_size);

    delete [] encrypted_size;
    delete [] res;
    delete [] encrypted_response;
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

int createCleartextHeader(char* buf, unsigned int s) {
    return sprintf(buf, "%d", s) + 1; // + 1 to include '\0'
}

char* encryptResponse(const char* password, char* plaintext, int plaintext_size, unsigned int * encrypted_size) {
    return static_cast<char*>(fs_encrypt(password, plaintext, plaintext_size, encrypted_size));
}
