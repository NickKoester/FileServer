#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "fs_server.h"
#include <unordered_map>
using namespace std;

int getMessageLength(const char *);



enum REQUEST_T { SESSION, READBLOCK, WRITEBLOCK, CREATE, DELETE };

struct sockaddr_in addr, cli_addr;

unordered_map<string, string> users;

const int HEADER_SIZE = (sizeof(char) * FS_MAXUSERNAME) + sizeof(unsigned) + sizeof(char);



int main(int argc, char *argv[])
{
/*  1. Read the list of usernames and passwords from stdin
 *  TODO: 2. Initialize the list of free disk blocks
 *  3. Set up the socket and call listen.
 */

    string user;
    string password;
    while (cin >> user) {
        cin >> password;
        users[user] = password;
    }




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

        char test[4096];
        memset(test, 0, 4096);
        int test_rc = recv(msg_fd, test, 4096, 0);
        cout << "TEST_RC: " << test_rc << endl;
        printf("MESSAGE:    %s\n", test);
 
       // auto send_data = to_string(getMessageLength(test)).c_str();
       auto testsend = "44";
       send(msg_fd, testsend, strlen(testsend), 0);        

       char test2[4096];
       memset(test2, 0, 4096);
       int test_rc2 = recv(msg_fd, test2, 4096, 0);
       cout << "TEST_RC 2:    " << test_rc2 << endl;
       printf("MESSAGE: %s", test2);
       exit(1); 





        //Getting the header -- get rid of magic number - this was just a guess 
        char buff[HEADER_SIZE];
        memset(buff, 0, sizeof(buff));
        int bytes_rcvd = 0;
        while (bytes_rcvd < HEADER_SIZE) {
            int rc = recv(msg_fd, buff + bytes_rcvd, HEADER_SIZE- bytes_rcvd, 0);
            cout << rc << endl;
            if (rc == -1) {
                cerr << "Error getting message\n";
            }
            if (rc == 0) break;
            bytes_rcvd += rc;
        }
        printf("Header:\t%s\n", buff);
        
        int messageLength = getMessageLength(buff);
        cout << "MESSAGE LENGTH: " << messageLength << endl;
        
        char *mess = new char[messageLength];
       // memset(mess, 0, sizeof(mess));
        int bytes_rcvd2 = 0;
        while (bytes_rcvd2 < messageLength) {
            int rc = recv(msg_fd, mess + bytes_rcvd2, messageLength - bytes_rcvd2, 0);
            if (rc == -1) {
                cerr << "Error getting message\n";
            }
            if (rc == 0) break;
            bytes_rcvd2 += rc;
        }
        printf("Encrypted Message:\t%s\n", mess);
        delete[] mess;
       
        close(msg_fd);

    //TODO: this shit
    REQUEST_T requestType = SESSION;
    switch(requestType)
    {
        case SESSION:
           cout << "Session Request\n";
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
    }
       
    return 0;
}

int getMessageLength(const char *buff) {
    int i = 0;
    while(buff[i++] != ' ');
    int length = atoi(buff + i);
    return length;
}
//REQUEST_T blowjob(char *buff);

































