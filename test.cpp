#include <iostream>
#include <cstdlib>
#include "fs_client.h"

using namespace std;

int main(int argc, char *argv[])

{
    const char *catwords = "meow meow meow Meow meow meow meow meow meow meow meow meow meow meow meow";
    char buff[FS_BLOCKSIZE];

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
    fs_writeblock("user1", "password1", session, seq++, "/secret_cat_pics/cute_cat1.jpg", 0, catwords);
    fs_readblock("user1", "password1", session, seq++, "/secret_cat_pics/cute_cat1.jpg", 0, buff);
    buff[10] = '\0';
    printf("%s", buff);
    system("./showfs");
    fs_delete("user1", "password1", session, seq++, "/secret_cat_pics/cute_cat1.jpg");
    fs_delete("user1", "password1", session, seq++, "/secret_cat_pics/cute_cat2.jpg");
    fs_delete("user1", "password1", session, seq++, "/secret_cat_pics");
    system("./showfs");

    return 0;
}
