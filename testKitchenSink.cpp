#include <iostream>
#include <cstdlib>
#include "fs_client.h"

using namespace std;

int main(int argc, char *argv[])

{
    char *server;
    int server_port;
    unsigned int session, seq=0;

    const char *writedata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";

    char readdata[FS_BLOCKSIZE];

    if (argc != 3) {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);
    for (size_t i = 0; i < 1000; i++) {
        fs_session("user1", "password1", &session, seq++);
        fs_create("user1", "password1", session, seq++, "/dir", 'd');
        fs_create("user1", "password1", session, seq++, "/dir/file", 'f');
        fs_create("user1", "password1", session, seq++, "/dir/cats", 'd');
        fs_create("user1", "password1", session, seq++, "/dir/cats/dogs", 'd');
        fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/mouse", 'd');
        fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/dog1", 'f');
        fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/dog2", 'f');
        fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/dog3", 'f');
        fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/dog4", 'f');
        fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/dog5", 'f');
        fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/dog6", 'f');
        fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/dog7", 'f');
        fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/dog8", 'f');
        fs_create("user1", "password1", session, seq++, "/dir/cats/dogs", 'd');
        fs_create("user1", "password1", session, seq++, "/dir/cats/cat1", 'd');
        fs_create("user1", "password1", session, seq++, "/dir/cats/cat2", 'd');
        fs_create("user1", "password1", session, seq++, "/dir/cats/cat3", 'd');
        fs_create("user1", "password1", session, seq++, "/dir/cats/cat4", 'd');
        fs_create("user1", "password1", session, seq++, "/dir/cats/cat6", 'd');
        fs_create("user1", "password1", session, seq++, "/dir/cats/cat7", 'd');
        fs_create("user1", "password1", session, seq++, "/dir/cats/cat8", 'd');
        fs_create("user1", "password1", session, seq++, "/dir/file", 'f');
        fs_create("user1", "password1", session, seq++, "/dir/file", 'f');
        fs_create("user1", "password1", session, seq++, "/dir/file", 'f');
        fs_create("user1", "password1", session, seq++, "/dir/file", 'f');
        fs_create("user1", "password1", session, seq++, "/dir/file", 'f');
        fs_create("user1", "password1", session, seq++, "/dir/file", 'f');
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 0, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 1, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 2, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 3, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 4, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 5, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 6, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 7, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 9, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 8, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 9, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 10, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 11, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 12, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 13, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 14, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 15, writedata);
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", 16, writedata);
        fs_readblock("user1", "password1", session, seq++, "/dir/file", 8, readdata);
        fs_delete("user1", "password1", session, seq++, "/dir/file");
        fs_delete("user1", "password1", session, seq++, "/dir");
    }
}
