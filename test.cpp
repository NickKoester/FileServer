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
    fs_create("user1", "password1", session, seq++, "/secret_cat_pics", 'd');
    fs_create("user1", "password1", session, seq++, "/secret_cat_pics/cute_cat1.jpg", 'f');
    fs_create("user1", "password1", session, seq++, "/secret_cat_pics/cute_cat2.jpg", 'f');
    fs_create("user1", "password1", session, seq++, "/secret_cat_pics/cute_cat3.jpg", 'f');
    fs_create("user1", "password1", session, seq++, "/secret_cat_pics/more_stuff", 'd');
    fs_create("user1", "password1", session, seq++, "/secret_cat_pics/more_stuff/cats.jpg", 'f');
    system("./showfs");
    return 0;
}
