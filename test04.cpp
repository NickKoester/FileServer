#include <iostream>
#include <cstdlib>
#include <cstring>
#include "fs_client.h"

using namespace std;

int main(int argc, char* argv[]) 
{
    char words[FS_BLOCKSIZE];
    memset(words, 0, FS_BLOCKSIZE);
    strcpy(words, "greetings");

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
    fs_create(username, password, session, seq++, "/andrew/hi", 'd');
    fs_delete(username, password, session, seq++, "/andrew/hi");
    fs_create(username, password, session, seq++, "/andrew/hi", 'f');
    fs_delete(username, password, session, seq++, "/andrew/hi");
    fs_create(username, password, session, seq++, "/andrew/bye", 'd');

    unsigned int ses = 0;
    unsigned int seq2= 0;
    fs_session("user2", "password2", &ses, seq2);
    fs_writeblock("user2", "password2", ses, seq2++, "/andrew/hi", 0, words);  
    fs_readblock("user2", "password2", ses, seq2++, "/andrew/hi", 1, buf);
   

    fs_writeblock(username, password, session, seq++, "/andrew/hi", 0, words);
    
    fs_readblock("user2", "password2", ses, seq2++, "/andrew/hi", 0, buf);
 
    fs_create("user2", "password2", ses, seq2++, "/andrew/bye/hello", 'f');
    fs_create("user2", "password2", ses, seq2++, "/andrew/bye/lol", 'd');

    fs_writeblock(username, password, session, seq++, "/andrew/hi", 1, words);
    fs_readblock(username, password, session, seq++, "/andrew/hi", 0, buf);
    fs_readblock(username, password, session, seq++, "/andrew/hi", 1, buf);
    fs_readblock(username, password, session, seq++, "/andrew/hi", 2, buf);
    fs_writeblock(username, password, session, seq++, "/andrew/hi", 3, words);
    
    fs_delete("user2", "password2", ses, seq2++, "/andrew/hi");

    fs_delete(username, password, session, seq++, "/andrew/hi");
    fs_delete(username, password, session, seq++, "/");
   
    unsigned int sess = 0;
    unsigned int seq3 = 0;
    fs_session("user3", "wrong", &sess, seq3++);
    sess = 0;
    fs_session("nope", "password3", &sess, seq3++);     
    sess = 1;
    fs_session("user3", "password3", &sess, seq3++); 

    return 0;
}
