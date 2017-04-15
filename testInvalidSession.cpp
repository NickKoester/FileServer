#include <iostream>
#include <cstdlib>
#include "fs_client.h"

using namespace std;

int main(int argc, char *argv[])

{
    char *server;
    int server_port;
    unsigned int session1 = 0, session2 = 0, seq1 =0, seq2 = 0;

    if (argc != 3) {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);
    fs_create("user1", "password1", session1, seq1, "/dir", 'd');
    fs_session("user1", "password1", &session1, seq1);
    fs_create("user1", "password1", session1, seq1, "/dir", 'd');

    fs_session("user1", "password1", &session1, seq1++);
    fs_create("user1", "password1", 12, seq1++, "/dir", 'd');

    fs_session("user2", "password2", &session2, seq2++);
    fs_create("user2", "password2", session2, seq2++, "/cats/file", 'f');
}
