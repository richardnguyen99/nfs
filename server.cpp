#include <iostream>     // cout, perror,
#include <cstring>      //memset
#include <cstdlib>      // atoi, exit, EXIT_FAILURE
#include <sys/types.h>  // addrinfo
#include <sys/socket.h> // socklen_t, SOCK_STREAM, PF_INET, AI_PASSIVE
#include <netdb.h>      //freeaddrinfo, getaddrinfo
#include <netinet/in.h>
//#include <string>

#include "FileSys.h"

using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage: ./nfsserver port#\n";
        return -1;
    }
    int port = atoi(argv[1]);

    // networking part: create the socket and accept the client connection
    // int sock;
    int serverfd, clientfd;
    socklen_t addrlen;

    // man 3 getaddrinfo
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, argv[1], &hints, &result) != 0)
    {
        perror("getaddrinfo failed");
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        cout << "Address: " << rp->ai_addr << endl;
        cout << "Address length: " << rp->ai_addrlen << endl;
        cout << "Family: " << rp->ai_family << endl;
    }

    freeaddrinfo(result);

    // mount the file system
    // FileSys fs;
    // fs.mount(serverfd); //assume that sock is the new socket created
    ////for a TCP connection between the client and the server.

    ////loop: get the command from the client and invoke the file
    ////system operation which returns the results or error messages back to the clinet
    ////until the client closes the TCP connection.

    ////close the listening socket

    ////unmout the file system
    // fs.unmount();

    return 0;
}
