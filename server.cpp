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
#include "LockManager.h"
#include "UserManager.h"
using namespace std;


struct sockaddr_in addr, cli_addr;

UserManager userManager;
BlockManager blockManager;
SessionManager sessionManager;
LockManager lockManager;

int main(int argc, char *argv[])
{
    userManager.initialize();
    blockManager.initialize();

    /******* SOCKET STUFF *********/
    int port_number = argc > 1 ? stoi(argv[1]) : 0;

    // Creating a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        cout_lock.lock();
        cerr << "Error opening stream socket\n";
        cout_lock.unlock();
        return -1;
    }
    
    memset(&addr, 0, sizeof(addr));

    int enable = 1;
    // Reusing ports or something
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
        cout_lock.lock();
        cerr << "Error in setsockopt\n";
        cout_lock.unlock();
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_number);
    
    if (bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        cout_lock.lock();
        cerr << "Error binding stream socket\n";
        cout_lock.unlock();
        return -1;
    }

    socklen_t length = sizeof(addr);
    if (getsockname(sockfd, (struct sockaddr *) &addr, &length) == -1) {
        cout_lock.lock();
        cerr << "Error getting socket name\n";
        cout_lock.unlock();
        return -1;
    }

    cout_lock.lock();
    cout << "\n@@@ port " << ntohs(addr.sin_port) << endl;
    cout_lock.unlock();

    listen(sockfd, 10);
    /********* SERVER STUFF *******/
    while (1) {
        int msg_fd = accept(sockfd, nullptr, nullptr);
        thread(requestHandler, msg_fd).detach();
        //requestHandler(msg_fd);
    }
       
    return 0;
}
