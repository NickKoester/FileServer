#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <unordered_map>
#include <thread>


#include "fs_server.h"
#include "fs_crypt.h"

#include "serverHelpers.h"
#include "requests.h"
#include "BlockManager.h"
#include "SessionManager.h"
using namespace std;


struct sockaddr_in addr, cli_addr;

unordered_map<string, string> users;


BlockManager blockManager;
SessionManager sessionManager;

int main(int argc, char *argv[])
{
    initializeUsers(users);
    blockManager.initialize();

    /******* SOCKET STUFF *********/
    int port_number = argc > 1 ? stoi(argv[1]) : 0;

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
        //thread(requestHandler, msg_fd).join();
        requestHandler(msg_fd);
    }
       
    return 0;
}
