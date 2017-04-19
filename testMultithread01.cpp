#include <iostream>
#include <cstdlib>
#include "fs_client.h"
#include <thread>
#include <cstring>
using namespace std;

unsigned session = 0;
unsigned seq = 0;


void test()
{
    char buf[FS_BLOCKSIZE];
    fs_create("user1", "password1", session, seq++, "/andrew", 'f');
    fs_create("user1", "password1", session, seq++, "/andrew/gre", 'f');
    fs_create("user1", "password1", session, seq++, "/andrew/hi", 'd');
    fs_readblock("user1", "password1", session, seq++, "/andrew/hi",0, buf);
    fs_delete("user1", "password1", session, seq++, "/andrew/hi");
    fs_delete("user1", "password1", session, seq++, "/andrew/hello");
}

void test2()
{
    char words[FS_BLOCKSIZE];
    memset(words, 0, FS_BLOCKSIZE);
    strcpy(words, "HASIDOKASLDASODIJKLAD ASOIDJAOSKLDMX ASIDOJLM");
    fs_writeblock("user1", "password1", session, seq++, "/andrew", 0, words);
    fs_create("user1", "password1", session, seq++, "/andrew/hi", 'f');
    fs_create("user1", "password1", session, seq++, "/andrew/hello", 'd');
    fs_writeblock("user1", "password1", session, seq++, "/andrew/hi", 0, words);    
    fs_create("user1", "password1", session, seq++, "/andrew/hello/greetings", 'f');
    fs_writeblock("user1", "password1", session, seq++, "/andrew/hi", 1, words);    
    fs_writeblock("user1", "password1", session, seq++, "/andrew/hi", 2, words);    
    fs_writeblock("user1", "password1", session, seq++, "/andrew/hi", 3, words);    
    fs_writeblock("user1", "password1", session, seq++, "/andrew/hi", 4, words);    
    fs_writeblock("user1", "password1", session, seq++, "/andrew/hi", 5, words);   
    fs_writeblock("user1", "password1", session, seq++, "/andrew/hi", 6, words);    
    fs_delete("user1", "password1", session, seq++, "/andrew");
}

void test3()
{
    unsigned ses = 0;
    unsigned seqe = 0;
    char words[FS_BLOCKSIZE];
    memset(words, 0, FS_BLOCKSIZE);
    strcpy(words, "IJASDJK OASD AODSN ADJSJKLDAOISJ ASODJN ASDJ NADSJK DASOJK ADSLKN ADKLNADSL NADS NADLKNASDKO ADSKASDKMASD UQWNDJKMA AOSIJlmx");
    fs_session("user2", "password2", &ses, seqe++);
    fs_create("user2", "password2", ses, seqe++, "/doge", 'd');
    fs_create("user2", "password2", ses, seqe++, "/doge/h", 'f');
    fs_writeblock("user2", "password2", ses, seqe++, "/doge/h", 0, words);
    char buf[FS_BLOCKSIZE];
    fs_readblock("user2", "password2", ses, seqe++, "/doge/h", 0, buf);

}

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
    fs_clientinit(server, port);
    fs_session("user1", "password1", &session, seq++);
    
    std::thread f(test);
    std::thread b(test2);
    std::thread c(test3);
    f.join();
    b.join();    
    c.join();
    return 0;
}
