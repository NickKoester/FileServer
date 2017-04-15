#include <iostream>
#include <cstdlib>
#include <cstring>
#include "fs_client.h"

using namespace std;

int main(int argc, char* argv[]) 
{
    char *server;
    int port;

    if (argc != 3) {
        cout << "error" << endl;
        exit(1);
    }
    server = argv[1];
    port = atoi(argv[2]);

    fs_clientinit(server, port);

    return 0;
}
