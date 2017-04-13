#include <iostream>
#include <cstdlib>
#include "fs_client.h"

using namespace std;

int main(int argc, char* argv[]) 
{

    const char *words = "hello from the other side";

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

    char buf[FS_BLOCKSIZE];

    fs_clientinit(server, port);
    fs_session(username, password, &session, seq++);
    fs_create(username, password, session, seq++, "/andrew", 'd');
    fs_create(username, password, session, seq++, "/mark", 'd');
    fs_create(username, password, session, seq++, "/nick", 'd');
    fs_create(username, password, session, seq++, "/andrew/pics", 'd');
    fs_create(username, password, session, seq++, "/andrew/pics/hi", 'f');
    fs_writeblock(username, password, session, seq++, "/andrew/pics/hi", 0, words);
    fs_readblock(username, password, session, seq++, "/andrew/pics/hi", 0, buf);
    //printf("buffer: %s\n", buf);
    system("./showfs");
    fs_delete(username, password, session, seq++, "/andrew/pics/hi");
    fs_delete(username, password, session, seq++, "/andrew/pics");
    fs_delete(username, password, session, seq++, "/andrew");
    system("./showfs");
    
    return 0;
}
