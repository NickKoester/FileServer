#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "fs_server.h"
using namespace std;

enum REQUEST_T { SESSION, READBLOCK, WRITEBLOCK, CREATE, DELETE };

struct sockaddr_in addr, cli_addr;

int main(int argc, char *argv[])
{
/*  1. Read the list of usernames and passwords from stdin
 *  2. Initialize the list of free disk blocks
 *  3. Set up the socket and call listen.
 */


    /******* SOCKET STUFF *********/
    int port_number = 0;
    if(argc > 1) {
        port_number = stoi(argv[1]);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        cerr << "Error opening stream socket\n";
        return -1;
    }
    
    memset(&addr, 0, sizeof(addr));

    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
        cerr << "Error in setsockopt\n";
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_number);
    
    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        cerr << "Error binding stream socket\n";
        return -1;
    }

    socklen_t length = sizeof(addr);
    if (getsockname(sock, (struct sockaddr *) &addr, &length) == -1) {
        cerr << "Error getting socket name\n";
        return -1;
    }

    cout << "\n@@@ port " << ntohs(addr.sin_port) << endl;

    listen(sock, 10);

    /********* SERVER STUFF *******/
    while (1) {
        //IDK about them args my thinking is that we dont need a struct but
        //Im not the socket expert so feel free to correct me
        int newsock = accept(sock, (struct sockaddr*) nullptr, nullptr);

        //Getting the header -- get rid of magic number - this was just a guess 
        int header_size = 20;
        char *buff = new char[header_size];
        int bytes_rcvd = 0;
        while (bytes_rcvd < header_size) {
            int rc = recv(newsock, buff + bytes_rcvd, header_size - bytes_rcvd, 0);
            if (rc <= 0) break;
            bytes_rcvd += rc;
        }
        printf("Header:\t%s\n", buff);
        delete[] buff;
        close(newsock);

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
