#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <dirent.h>
#include <sys/stat.h>

enum {BUFFSIZE = 512};
typedef enum {QUIT, LIST, PULL, PUSH} COMMAND;

void send_listing(int socket_fd, char * buffer);

int main(int argc, char * argv[])
{
    (void)argc;
    int socket_fd = atoi(argv[1]);
    char * buffer = calloc(sizeof(char), BUFFSIZE);
    unsigned int data = 0;
    while (1)
    {
        memset(buffer, 0, BUFFSIZE);
        read(socket_fd, &data, sizeof(int));
        
        switch(data)
        {
            case QUIT:
                free(buffer);
                shutdown(socket_fd, SHUT_RDWR);
                close(socket_fd);
                return 0;
            case LIST: ;
                send_listing(socket_fd, buffer);
                break;
            case PULL:
                break;
            case PUSH:
                break;
            default:
                break;
        }
    }
}

void send_listing(int socket_fd, char * buffer)
{
    memset(buffer, 0, BUFFSIZE);
    DIR * d;
    struct dirent * dir;
    d = opendir(".");
    
    if (d != NULL)
    {
        int file_index = 1;
        while((dir = readdir(d)) != NULL)
        {
            // Iterate through every file inside of the FTP directory
            struct stat file_stats;
            stat(dir->d_name, &file_stats);
            // Check if the file is a regular file (Ignore directories)
            if (S_ISREG(file_stats.st_mode))
            {
                // Check to see if you have enough space in the buffer
                size_t amt_left = BUFFSIZE - strlen(buffer);
                if (amt_left < strlen(dir->d_name))
                {
                    // Not enough space in buffer. Send what you have and reset the buffer
                    write(socket_fd, buffer, strlen(buffer));
                    memset(buffer, 0, BUFFSIZE);
                }
                // Concatenate the file to the buffer string
                strncat(buffer, dir->d_name, strlen(dir->d_name));
                // TODO: Change from \t and \n to just a space for client to handle
                file_index % 10 ? strcat(buffer, "\t") :  strcat(buffer, "\n");
                // Update the index
                file_index = (file_index + 1) % 10;
            }
        }
        // Done with directory so close
        closedir(d);
        // Check to see if there is enough space to append EOT to buffer
        if (BUFFSIZE - strlen(buffer) >= 1)
        {
            // Enough space just concat
            char EOT = 0x4;
            strncat(buffer, &EOT, sizeof(char));
        }
        else
        {
            // Not enough space write data to socket, reset memory, and put EOT in buffer
            write(socket_fd, buffer, strlen(buffer));
            memset(buffer, 0, BUFFSIZE);
            buffer[0] = 0x04;
        }
        // Write reamaing data into the buffer
        write(socket_fd, buffer, strlen(buffer));
    }
    
    return;
}
// END OF SOURCE

