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
    dirblock_t new_curr_dir_block;
    short i;

    // load the index of the new directory
    if ((i = this->get_block_index(name)) == -1)
    {
        this->response("503", "File does not exist", "");
        return;
    }

    // Read the content of the new block
    this->bfs.read_block(this->curr_dir_block.dir_entries[i].block_num, (void *)&new_curr_dir_block);

    if (new_curr_dir_block.magic != DIR_MAGIC_NUM)
    {
        this->response("500", "File is not a directory", "");
        return;
    }

    // Switching to the new directory block
    this->curr_dir = this->curr_dir_block.dir_entries[i].block_num;
    this->curr_dir_block = new_curr_dir_block;

    this->response("200", "OK", "success");
}

// switch to home directory
void FileSys::home()
{
    // Home directory has num of 1
    this->curr_dir = 1;
    this->bfs.read_block(this->curr_dir, (void *)&(this->curr_dir_block));

    this->response("200", "OK", "success");
}

// remove a directory
void FileSys::rmdir(const char *name)
{
    short i;
    dirblock_t rm_block;

    if ((i = this->get_block_index(name)) == -1)
    {
        this->response("503", "File does not exist", "");
        return;
    }

    short rm_block_num = this->curr_dir_block.dir_entries[i].block_num;
    this->bfs.read_block(rm_block_num, (void *)&rm_block);

    if (rm_block.magic != DIR_MAGIC_NUM)
    {
        this->response("500", "File is not a directory", "");
        return;
    }

    if (rm_block.num_entries > 0)
    {
        this->response("507", "Directory is not empty", "");
        return;
    }

    std::swap(this->curr_dir_block.dir_entries[i], this->curr_dir_block.dir_entries[this->curr_dir_block.num_entries - 1]);
    this->curr_dir_block.num_entries--;

    this->bfs.write_block(this->curr_dir, (void *)&(this->curr_dir_block));
    this->bfs.reclaim_block(rm_block_num);

    this->response("200", "OK", "success");
}

// list the contents of current directory
void FileSys::ls()
{
    std::string content = "";

    if (this->curr_dir_block.num_entries == 0)
    {
        this->response("200", "OK", "empty folder");
        return;
    }

    dirblock_t tmp;

    for (size_t i = 0; i < this->curr_dir_block.num_entries; i++)
    {
        content += this->curr_dir_block.dir_entries[i].name;
        bfs.read_block(this->curr_dir_block.dir_entries[i].block_num, (void *)&tmp);

        if (tmp.magic == DIR_MAGIC_NUM)
            content += "/";

        if (i < this->curr_dir_block.num_entries - 1)
            content += " ";
    }
    this->response("200", "OK", content);
}

// create an empty data file
void FileSys::create(const char *name)
{
    inode_t new_file;
    short new_block_num;

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

    if (!(new_block_num = this->bfs.get_free_block()))
    {
        this->response("505", "Disk is full", "");
        return;
    }

    new_file.size = 0;
    new_file.magic = INODE_MAGIC_NUM;
    bfs.write_block(new_block_num, (void *)&new_file);

    strcpy(this->curr_dir_block.dir_entries[this->curr_dir_block.num_entries].name, name);

    this->curr_dir_block.dir_entries[this->curr_dir_block.num_entries].block_num = new_block_num;
    this->curr_dir_block.num_entries++;

    this->bfs.write_block(this->curr_dir, (void *)&(this->curr_dir_block));

    this->response("200", "OK", "success");
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
    const std::string status_message = status + " " + msg + "\r\n";
    const std::string length_message = "Length:" + std::to_string(data.length()) + "\r\n";
    const std::string brline_message = "\r\n";
    const std::string result_message = data;

    const std::string res = status_message + length_message + brline_message + result_message;

    this->message(res);
}
