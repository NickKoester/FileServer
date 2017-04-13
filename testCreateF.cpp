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

    const char* username = "user1";
    const char* password = "password1";
    unsigned session = 0;
    unsigned seq = 0;


    fs_clientinit(server, port);
    fs_session(username, password, &session, seq++);
    fs_create(username, password, session, seq++, "/andrew", 'f');

    return 0;
}
