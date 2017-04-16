#include <iostream>
#include <cstdlib>
#include "fs_client.h"

using namespace std;

int main(int argc, char *argv[])

{
    char *server;
    int server_port;
    unsigned int session, seq=0;

    if (argc != 3) {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);
    fs_session("user1", "password1", &session, seq++);
    fs_create("user1", "password1", session, seq++, "/dir", 'd');
    fs_create("user1", "password1", session, seq++, "/dir/file1", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file2", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file3", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file4", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file5", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file6", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file7", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file8", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file9", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file10", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file11", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file12", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file13", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file14", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file15", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file16", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file17", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file18", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file19", 'f');
    fs_create("user1", "password1", session, seq++, "/dir/file20", 'f');
    fs_delete("user1", "password1", session, seq++, "/dir");
    fs_delete("user1", "password1", session, seq++, "/dir/file1");
    fs_delete("user1", "password1", session, seq++, "/dir/file2");
    fs_delete("user1", "password1", session, seq++, "/dir/file3");
    fs_delete("user1", "password1", session, seq++, "/dir/file4");
    fs_delete("user1", "password1", session, seq++, "/dir/file5");
    fs_delete("user1", "password1", session, seq++, "/dir/file6");
    fs_delete("user1", "password1", session, seq++, "/dir/file7");
    fs_delete("user1", "password1", session, seq++, "/dir/file8");
    fs_delete("user1", "password1", session, seq++, "/dir/file9");
    fs_delete("user1", "password1", session, seq++, "/dir/file10");
    system("./showfs");
    fs_create("user1", "password1", session, seq++, "/dir/file10", 'f');
    system("./showfs");
    fs_delete("user1", "password1", session, seq++, "/dir/file10");
    fs_delete("user1", "password1", session, seq++, "/dir/file11");
    fs_delete("user1", "password1", session, seq++, "/dir/file12");
    fs_delete("user1", "password1", session, seq++, "/dir/file13");
    fs_delete("user1", "password1", session, seq++, "/dir/file14");
    fs_delete("user1", "password1", session, seq++, "/dir/file15");
    fs_delete("user1", "password1", session, seq++, "/dir/file16");
    fs_delete("user1", "password1", session, seq++, "/dir/file17");
    fs_delete("user1", "password1", session, seq++, "/dir/file18");
    fs_delete("user1", "password1", session, seq++, "/dir/file19");
    fs_delete("user1", "password1", session, seq++, "/dir/file20");
}
