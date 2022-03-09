// CPSC 3500: File System
// Implements the file system commands that are available to the shell.

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
using namespace std;

#include "FileSys.h"
#include "BasicFileSys.h"
#include "Blocks.h"

// mounts the file system
void FileSys::mount(int sock)
{
    bfs.mount();
    curr_dir = 1; // by default current directory is home directory, in disk block #1
    bfs.read_block(curr_dir, (void *)&(this->curr_dir_block));
    fs_sock = sock; // use this socket to receive file system operations from the client and send back response messages
}

// unmounts the file system
void FileSys::unmount()
{
    bfs.unmount();
    close(fs_sock);
}

// make a directory
void FileSys::mkdir(const char *name)
{
    short new_block;
    dirblock_t new_directory;

    if (this->get_block_index(name) >= 0)
    {
        this->response("502", "File exists", "");
        return;
    }

    if (strlen(name) > MAX_FNAME_SIZE)
    {
        this->response("504", "File name is too long", "");
        return;
    }

    if (this->curr_dir_block.num_entries == MAX_DIR_ENTRIES)
    {
        this->response("506", "Directory is full", "");
        return;
    }

    if (!(new_block = bfs.get_free_block()))
    {
        this->response("505", "Disk is full", "");
        return;
    }

    new_directory.num_entries = 0;
    new_directory.magic = DIR_MAGIC_NUM;
    bfs.write_block(new_block, &new_directory);

    int i = 0;
    strcpy(this->curr_dir_block.dir_entries[this->curr_dir_block.num_entries].name, name);

    this->curr_dir_block.dir_entries[this->curr_dir_block.num_entries].block_num = new_block;
    this->curr_dir_block.num_entries++;

    this->bfs.write_block(this->curr_dir, &(this->curr_dir_block));

    this->response("200", "OK", "success");
}

// switch to a directory
void FileSys::cd(const char *name)
{
}

// switch to home directory
void FileSys::home()
{
}

// remove a directory
void FileSys::rmdir(const char *name)
{
}

// list the contents of current directory
void FileSys::ls()
{
}

// create an empty data file
void FileSys::create(const char *name)
{
}

// append data to a data file
void FileSys::append(const char *name, const char *data)
{
}

// display the contents of a data file
void FileSys::cat(const char *name)
{
}

// display the first N bytes of the file
void FileSys::head(const char *name, unsigned int n)
{
}

// delete a data file
void FileSys::rm(const char *name)
{
}

// display stats about file or directory
void FileSys::stat(const char *name)
{
}

// HELPER FUNCTIONS (optional)
short FileSys::get_block_index(const char *name)
{
    for (size_t i = 0; i < this->curr_dir_block.num_entries; i++)
    {
        if (strcmp(curr_dir_block.dir_entries[i].name, name) == 0)
            return i;
    }

    return -1;
}

void FileSys::message(std::string message)
{
    const char *buffer = message.c_str();
    size_t msglen = message.length();
    size_t total_sent_byte = 0;

    while (total_sent_byte < msglen)
    {
        int byte = send(this->fs_sock, (void *)(buffer + total_sent_byte), msglen - total_sent_byte, 0);

        if (byte == -1)
        {
            perror("send failed");
            break;
        }

        total_sent_byte += byte;
    }
}

void FileSys::response(std::string status, std::string msg, std::string data)
{
    this->message(status + " " + msg + "\r\n");
    this->message("Length: " + std::to_string(data.length()) + "\r\n");
    this->message("\r\n");
    this->message(data);
}
