#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "fs_server.h"



struct sockaddr_in addr, cli_addr;

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;
    
    bzero((char *) &addr, sizeof(addr));

    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) 
        return -1;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
//    addr.sin_port = htons(port_number);

    bind(sock, (struct sockaddr*) &addr, sizeof(addr));
    
    listen(sock, 5);
    socklen_t clilen;
    clilen = sizeof(cli_addr);

    int newsockfd = accept(sock, (struct sockaddr*) &cli_addr, &clilen);
    char buffer[256];
    bzero(buffer, 256);
    int n = read(newsockfd, buffer, 255);
    printf("%s\n", buffer);
    close(newsockfd);
    close(sock);






    return 0;
}
