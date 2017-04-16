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
    fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/dog9", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/puppy1", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/puppy2", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/puppy3", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/puppy4", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/puppy5", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/puppy6", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/puppy7", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/puppy8", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/cats/dogs/puppy9", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/cats/dogs", 'd');
    fs_create("user1", "password1", session, seq++, "/dir/cats/cat1", 'd');
    fs_create("user1", "password1", session, seq++, "/dir/cats/cat2", 'd');
    fs_create("user1", "password1", session, seq++, "/dir/cats/cat3", 'd');
    fs_create("user1", "password1", session, seq++, "/dir/cats/cat4", 'd');

    for (size_t i = 0; i < 126; i++) {
        fs_writeblock("user1", "password1", session, seq++, "/dir/file", i, writedata);
    }

    fs_readblock("user1", "password1", session, seq++, "/dir/file", 0, readdata);
    fs_delete("user1", "password1", session, seq++, "/dir/file");
    system("./showfs");
    fs_delete("user1", "password1", session, seq++, "/dir/cats/dogs/dog8");
    fs_delete("user1", "password1", session, seq++, "/dir/cats/dogs/dog9");
    fs_delete("user1", "password1", session, seq++, "/dir/cats/dogs/puppy1");
    fs_delete("user1", "password1", session, seq++, "/dir/cats/dogs/puppy2");
    fs_delete("user1", "password1", session, seq++, "/dir/cats/dogs/puppy3");
    fs_delete("user1", "password1", session, seq++, "/dir/cats/dogs/puppy4");
    fs_delete("user1", "password1", session, seq++, "/dir/cats/dogs/puppy5");
    fs_delete("user1", "password1", session, seq++, "/dir/cats/dogs/puppy6");
    fs_delete("user1", "password1", session, seq++, "/dir");
}
