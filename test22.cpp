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
    fs_create("user1", "password1", session, seq++, "/one", 'd');
    fs_create("user1", "password1", session, seq++, "/two", 'f');
    fs_create("user1", "password1", session, seq++, "/three", 'f');
    fs_create("user1", "password1", session, seq++, "/four", 'd');
    fs_delete("user1", "password1", session, seq++, "/two");
    fs_create("user1", "password1", session, seq++, "/five", 'f');
    fs_delete("user1", "password1", session, seq++, "/three");
    fs_delete("user1", "password1", session, seq++, "/one");
    fs_delete("user1", "password1", session, seq++, "/four");
    fs_delete("user1", "password1", session, seq++, "/five");
    fs_create("user1", "password1", session, seq++, "/six", 'f');
    fs_delete("user1", "password1", session, seq++, "/six");

}
