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
            struct stat file_stats;
            stat(dir->d_name, &file_stats);
            if (S_ISREG(file_stats.st_mode))
            {
                size_t amt_left = BUFFSIZE - strlen(buffer);
                if (amt_left < strlen(dir->d_name))
                {
                    write(socket_fd, buffer, strlen(buffer));
                    memset(buffer, 0, BUFFSIZE);
                }

                strncat(buffer, dir->d_name, strlen(dir->d_name));
                file_index % 10 ? strcat(buffer, "\t") :  strcat(buffer, "\n");
                file_index = (file_index + 1) % 10;
            }
        }
        write(socket_fd, buffer, strlen(buffer));
        sleep(1);
        memset(buffer, 0, BUFFSIZE);
        write(socket_fd, buffer, 1);
        closedir(d);
    }
    
    return;
}
// END OF SOURCE

