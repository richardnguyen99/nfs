// CPSC 3500: Shell
// Implements a basic shell (command line interface) for the file system

#include <iostream> // cout, cerr, endl
#include <fstream>
#include <sstream>

#include "Shell.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

static const std::string PROMPT_STRING = "NFS> "; // shell prompt

// Mount the network file system with server name and port number in the format of server:port
void Shell::mountNFS(std::string fs_loc)
{
    // create the socket cs_sock and connect it to the server and port specified in fs_loc
    // if all the above operations are completed successfully, set is_mounted to true
    struct addrinfo hints;
    struct addrinfo *result;

    int sockfd;

    bool flag = false;
    int pos = fs_loc.find(":");
    std::string host = fs_loc.substr(0, pos);
    std::string port = fs_loc.substr(pos + 1);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // Try to find the available addresses with specified host and port
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0)
    {
        perror("getaddrinfo failed");
    }

    struct addrinfo *rp;
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        if ((sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1)
            perror("socket failed");
        else
        {
            if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == -1)
                perror("connect failed");
            else
            {
                flag = true;
                break; // connecting to server successfully
            }
        }
    }

    if (rp == NULL)
    {
        cerr << "No avaiable address to connect sockets" << endl;
    }

    this->cs_sock = sockfd;
    if (flag)
        this->is_mounted = flag;

    freeaddrinfo(result);
}

// Unmount the network file system if it was mounted
void Shell::unmountNFS()
{
    if (this->is_mounted)
        this->is_mounted = false;

    // close the socket if it was mounted
    if (close(this->cs_sock) == -1)
        perror("close error");
}

void Shell::write_msg(const std::string &msg)
{
    const std::string stdmsg = msg + "\r\n";
    const char *buffer = stdmsg.c_str();
    int msglen = stdmsg.length();
    int total_write_byte = 0;

    while (total_write_byte < msglen)
    {
        int byte = send(this->cs_sock, (void *)(buffer + total_write_byte), msglen - total_write_byte, 0);

        if (byte == -1)
        {
            perror("send failed");
            break;
        }

        total_write_byte += byte;
    }
}

std::string Shell::read_msg()
{
    char buffer[this->BUFFER_SIZE];
    int linecnt = 0;
    int total_read_byte = 0;
    int msglen = INT32_MAX; // Don't know msglen before hand
    std::string msg;
    std::string read_string = "";
    std::string status = "";

    // Standard response always has 4 lines of message
    while (total_read_byte < msglen)
    {

        int msgbreak = 0;
        int byte = recv(this->cs_sock, (void *)buffer, this->BUFFER_SIZE, 0);

        if (byte == -1)
        {
            perror("recv failed");
            break;
        }

        buffer[byte] = '\0';
        read_string = std::string(buffer);
        msg += read_string;

        if (linecnt == 3)
        {
            total_read_byte += byte;
        }

        msgbreak = msg.find("\r\n");

        // At the last message, msgbreak should be -1
        while (msgbreak >= 0)
        {
            if (linecnt == 0)
                status = msg.substr(0, msgbreak);
            if (linecnt == 1)
            {
                // Unsure how many place there is in the length message
                // But it can be identified between end of "Length:" and
                // beginning of  "\r\n"
                msglen = std::stoi(msg.substr(7, msgbreak - 7));
            }

            // Skip \r\n character
            // cout << msg;
            msg = msg.substr(msgbreak + 2);
            if (linecnt == 2)
                total_read_byte = msg.length();

            linecnt++;
            msgbreak = msg.find("\r\n");
        }
    }

    if (msglen == 0)
        msg = status;

    return msg;
}

// Remote procedure call on mkdir
void Shell::mkdir_rpc(std::string dname)
{
    const std::string msg = "mkdir " + dname;
    this->write_msg(msg);

    std::string res = this->read_msg();
    cout << res << endl;
}

// Remote procedure call on cd
void Shell::cd_rpc(std::string dname)
{
    const std::string msg = "cd " + dname;
    this->write_msg(msg);

    std::string res = this->read_msg();
    cout << res << endl;
}

// Remote procedure call on home
void Shell::home_rpc()
{
    const std::string msg = "home";
    this->write_msg(msg);

    std::string res = this->read_msg();
    cout << res << endl;
}

// Remote procedure call on rmdir
void Shell::rmdir_rpc(std::string dname)
{
    const std::string msg = "rmdir " + dname;
    this->write_msg(msg);

    std::string res = this->read_msg();
    cout << res << endl;
}

// Remote procedure call on ls
void Shell::ls_rpc()
{
    const std::string msg = "ls";
    this->write_msg(msg);

    std::string res = this->read_msg();
    cout << res << endl;
}

// Remote procedure call on create
void Shell::create_rpc(std::string fname)
{
    const std::string msg = "create " + fname;
    this->write_msg(msg);

    std::string res = this->read_msg();
    cout << res << endl;
}

// Remote procedure call on append
void Shell::append_rpc(std::string fname, std::string data)
{
    const std::string msg = "append " + fname + " " + data;
    this->write_msg(msg);

    std::string res = this->read_msg();
    cout << res << endl;
}

// Remote procesure call on cat
void Shell::cat_rpc(std::string fname)
{
    const std::string msg = "cat " + fname;
    this->write_msg(msg);

    std::string res = this->read_msg();
    cout << res << endl;
}

// Remote procedure call on head
void Shell::head_rpc(std::string fname, int n)
{
    const std::string msg = "head " + fname + " " + std::to_string(n);
    this->write_msg(msg);

    std::string res = this->read_msg();
    cout << res << endl;
}

// Remote procedure call on rm
void Shell::rm_rpc(std::string fname)
{
    const std::string msg = "rm " + fname;
    this->write_msg(msg);

    std::string res = this->read_msg();
    cout << res << endl;
}

// Remote procedure call on stat
void Shell::stat_rpc(std::string fname)
{
    const std::string msg = "stat " + fname;
    this->write_msg(msg);

    std::string res = this->read_msg();
    cout << res << endl;
}

// Executes the shell until the user quits.
void Shell::run()
{
    // make sure that the file system is mounted
    if (!is_mounted)
        return;

    // continue until the user quits
    bool user_quit = false;
    while (!user_quit)
    {

        // print prompt and get command line
        std::string command_str;
        cout << PROMPT_STRING;
        std::getline(cin, command_str);

        // execute the command
        user_quit = execute_command(command_str);
    }

    // unmount the file system
    unmountNFS();
}

// Execute a script.
void Shell::run_script(char *file_name)
{
    // make sure that the file system is mounted
    if (!is_mounted)
        return;
    // open script file
    std::ifstream infile;
    infile.open(file_name);
    if (infile.fail())
    {
        cerr << "Could not open script file" << endl;
        return;
    }

    // execute each line in the script
    bool user_quit = false;
    std::string command_str;
    getline(infile, command_str, '\n');
    while (!infile.eof() && !user_quit)
    {
        cout << PROMPT_STRING << command_str << endl;
        user_quit = execute_command(command_str);
        getline(infile, command_str);
    }

    // clean up
    unmountNFS();
    infile.close();
}

// Executes the command. Returns true for quit and false otherwise.
bool Shell::execute_command(std::string command_str)
{
    // parse the command line
    struct Command command = parse_command(command_str);

    // look for the matching command
    if (command.name == "")
    {
        return false;
    }
    else if (command.name == "mkdir")
    {
        mkdir_rpc(command.file_name);
    }
    else if (command.name == "cd")
    {
        cd_rpc(command.file_name);
    }
    else if (command.name == "home")
    {
        home_rpc();
    }
    else if (command.name == "rmdir")
    {
        rmdir_rpc(command.file_name);
    }
    else if (command.name == "ls")
    {
        ls_rpc();
    }
    else if (command.name == "create")
    {
        create_rpc(command.file_name);
    }
    else if (command.name == "append")
    {
        append_rpc(command.file_name, command.append_data);
    }
    else if (command.name == "cat")
    {
        cat_rpc(command.file_name);
    }
    else if (command.name == "head")
    {
        errno = 0;
        unsigned long n = strtoul(command.append_data.c_str(), NULL, 0);
        if (0 == errno)
        {
            head_rpc(command.file_name, n);
        }
        else
        {
            cerr << "Invalid command line: " << command.append_data;
            cerr << " is not a valid number of bytes" << endl;
            return false;
        }
    }
    else if (command.name == "rm")
    {
        rm_rpc(command.file_name);
    }
    else if (command.name == "stat")
    {
        stat_rpc(command.file_name);
    }
    else if (command.name == "quit")
    {
        return true;
    }

    return false;
}

// Parses a command line into a command struct. Returned name is blank
// for invalid command lines.
Shell::Command Shell::parse_command(std::string command_str)
{
    // empty command struct returned for errors
    struct Command empty = {"", "", ""};

    // grab each of the tokens (if they exist)
    struct Command command;
    std::istringstream ss(command_str);
    int num_tokens = 0;
    if (ss >> command.name)
    {
        num_tokens++;
        if (ss >> command.file_name)
        {
            num_tokens++;
            if (ss >> command.append_data)
            {
                num_tokens++;
                std::string junk;
                if (ss >> junk)
                {
                    num_tokens++;
                }
            }
        }
    }

    // Check for empty command line
    if (num_tokens == 0)
    {
        return empty;
    }

    // Check for invalid command lines
    if (command.name == "ls" ||
        command.name == "home" ||
        command.name == "quit")
    {
        if (num_tokens != 1)
        {
            cerr << "Invalid command line: " << command.name;
            cerr << " has improper number of arguments" << endl;
            return empty;
        }
    }
    else if (command.name == "mkdir" ||
             command.name == "cd" ||
             command.name == "rmdir" ||
             command.name == "create" ||
             command.name == "cat" ||
             command.name == "rm" ||
             command.name == "stat")
    {
        if (num_tokens != 2)
        {
            cerr << "Invalid command line: " << command.name;
            cerr << " has improper number of arguments" << endl;
            return empty;
        }
    }
    else if (command.name == "append" || command.name == "head")
    {
        if (num_tokens != 3)
        {
            cerr << "Invalid command line: " << command.name;
            cerr << " has improper number of arguments" << endl;
            return empty;
        }
    }
    else
    {
        cerr << "Invalid command line: " << command.name;
        cerr << " is not a command" << endl;
        return empty;
    }

    return command;
}
