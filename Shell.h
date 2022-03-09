// CPSC 3500: Shell
// Implements a basic shell (command line interface) for the file system

#ifndef SHELL_H
#define SHELL_H

#include <string>       // std::string, std::getline
#include <cstring>      // memset
#include <netdb.h>      // addrinfo, getaddrinfo, freeaddrinfo
#include <unistd.h>     // close
#include <sys/types.h>  // PF_INET, SOCK_STREAM
#include <sys/socket.h> // socket, connect, send
#include <netinet/in.h>

// Shell
class Shell
{

public:
    // constructor, do not change it!!
    Shell() : cs_sock(-1), is_mounted(false)
    {
    }

    // Mount a network file system located in host:port, set is_mounted = true if success
    void mountNFS(std::string fs_loc); // fs_loc must be in the format of server:port

    // unmount the mounted network file syste,
    void unmountNFS();

    // Executes the shell until the user quits.
    void run();

    // Execute a script.
    void run_script(char *file_name);

private:
    static constexpr std::size_t BUFFER_SIZE = 1024;

    int cs_sock; // socket to the network file system server

    bool is_mounted; // true if the network file system is mounted, false otherise

    // data structure for command line
    struct Command
    {
        std::string name;        // name of command
        std::string file_name;   // name of file
        std::string append_data; // append data (append only)
    };

    // Executes the command. Returns true for quit and false otherwise.
    bool execute_command(std::string command_str);

    // Parses a command line into a command struct. Returned name is blank
    // for invalid command lines.
    struct Command parse_command(std::string command_str);

    // Remote procedure call on mkdir
    void mkdir_rpc(std::string dname);

    // Remote procedure call on cd
    void cd_rpc(std::string dname);

    // Remote procedure call on home
    void home_rpc();

    // Remote procedure call on rmdir
    void rmdir_rpc(std::string dname);

    // Remote procedure call on ls
    void ls_rpc();

    // Remote procedure call on create
    void create_rpc(std::string fname);

    // Remote procedure call on append
    void append_rpc(std::string fname, std::string data);

    // Remote procesure call on cat
    void cat_rpc(std::string fname);

    // Remote procedure call on head
    void head_rpc(std::string fname, int n);

    // Remote procedure call on rm
    void rm_rpc(std::string fname);

    // Remote procedure call on stat
    void stat_rpc(std::string fname);

    // Internal function for shell to send message to server
    void write_msg(const std::string &command);

    // Internal function for shell to get message from server
    std::string read_msg();
};

#endif
