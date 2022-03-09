#include <iostream>     // cout, cerr, end, perror
#include <string>       // std::string
#include <sstream>      // std::istringstream
#include <cstring>      // memset
#include <cstdlib>      // atoi
#include <sys/types.h>  // PF_INET, SOCK_STREAM, AI_PASSIVE
#include <sys/socket.h> // socklen_t, sockaddr_storage, recv, socket, listen, accept, addrinfo, getaddrinfo, freeaddrinfo
#include <unistd.h>     // close, write
#include <netdb.h>
#include <netinet/in.h>

#include "FileSys.h"

using std::cerr;
using std::cout;
using std::endl;

void readfrom(int socket, std::string &command, int buf_size)
{
    char buffer[buf_size];
    // Reset the old command
    command = "";

    int read_byte = 0;
    std::string read_string = "";

    while (true)
    {
        read_byte = recv(socket, (void *)buffer, buf_size, 0);

        if (read_byte == -1)
        {
            cerr << "recv failed" << endl;
            break;
        }

        buffer[read_byte] = '\0';
        read_string = std::string(buffer);

        if (command.find("\r\n") >= 0)
            break;

        command += read_string;
    }

    command += read_string;
}

void sendto(int socket, const std::string &message)
{
    const std::string stdmsg = message + "\r\n";
    const char *buffer = stdmsg.c_str();

    int msglen = stdmsg.length();
    int total_sent_byte = 0;

    while (total_sent_byte < msglen)
    {
        int byte = write(socket, (void *)buffer, msglen - total_sent_byte);

        if (byte == -1 || byte == 0)
        {
            perror("write failed");
            break;
        }

        total_sent_byte += byte;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage: ./nfsserver port#\n";
        return -1;
    }

    // Defined variables
    constexpr int BUFFER_SIZE = 1024;

    socklen_t addr_size;
    struct addrinfo hints;   // Define information for creating & binding socket
    struct addrinfo *result; // Avaible addresses for socket
    struct addrinfo *rp;     // Result pointer to keep track when need to
    int sockfd, clientfd;
    struct sockaddr_storage accepted_addr;

    int port = atoi(argv[1]);

    // networking part: create the socket and accept the client connection
    // int sock; // change this line when necessary!

    // man 3 getaddrinfo
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Specify node is NULL and ai_flags is AI_PASSIVE
    // to find available addresses for binding.
    if (getaddrinfo(NULL, argv[1], &hints, &result) != 0)
    {
        perror("getaddrinfo failed");
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        if ((sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1)
        {
            perror("socket failed");
        }
        else
        {
            if (bind(sockfd, rp->ai_addr, rp->ai_addrlen) != 0)
                perror("connect failed");
            else
                break; // Creating and binding socket successfully
        }
    }

    if (rp == NULL)
    {
        cerr << "No avaiable address to bind sockets" << endl;
    }

    if (listen(sockfd, 0) != 0)
    {
        perror("listen failed");
    }

    addr_size = sizeof(accepted_addr);

    if ((clientfd = accept(sockfd, (struct sockaddr *)&accepted_addr, &addr_size)) == -1)
    {
        perror("accept failed");
    }

    freeaddrinfo(result); // No longer needed

    // mount the file system
    FileSys fs;
    fs.mount(clientfd); // assume that clientfd is the new socket created
                        // for a TCP connection between the client and the server.

    // loop: get the command from the client and invoke the file
    // system operation which returns the results or error messages back to the clinet
    // until the client closes the TCP connection.

    // Available commands have a standard structure
    // <cmd> <fname> <bytes>
    // where
    // cmd is the command name, must be provided
    // fname can be optionally used for file or directory name
    // bytes is for some specific commands, also optional.
    std::string usr_in = "";
    std::string cmd = "";
    std::string fname = "";
    std::string bytes = "";

    while (1)
    {
        readfrom(clientfd, usr_in, BUFFER_SIZE);

        std::istringstream input_stream(usr_in);

        if (input_stream >> cmd)
            if (input_stream >> fname)
                input_stream >> bytes;

        if (cmd == "mkdir")
            fs.mkdir(fname.c_str());
        else if (cmd == "cd")
            fs.cd(fname.c_str());
        else if (cmd == "home")
            fs.home();
        else if (cmd == "rmdir")
            fs.rmdir(fname.c_str());
        else if (cmd == "ls")
            fs.ls();
        else if (cmd == "create")
            fs.create(fname.c_str());
        else if (cmd == "append")
            fs.append(fname.c_str(), bytes.c_str());
        else if (cmd == "cat")
            fs.cat(fname.c_str());
        else if (cmd == "head")
        {
            cout << "$" << bytes << "$";
            fs.head(fname.c_str(), stoi(bytes));
        }
        else if (cmd == "rm")
            fs.rm(fname.c_str());
        else if (cmd == "stat")
            fs.stat(fname.c_str());
        else
            sendto(clientfd, "Invalid Command");
    }

    // close the listening socket
    close(sockfd);

    // unmout the file system
    fs.unmount();

    return 0;
}
