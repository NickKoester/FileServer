#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
    char *server;
    int server_port;
    unsigned session, seq=0;

    const char *writedata = "we hold these truths to be self-evident";

    char readdata[FS_BLOCKSIZE];

    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);
    





}
