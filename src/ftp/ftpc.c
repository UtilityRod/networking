#include "socket_factory.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

enum {BUFFSIZE = 512};
typedef enum {QUIT, LIST, PULL, PUSH} COMMAND;

unsigned int get_command(char * buffer);
void read_listing(int socket_fd, char * buffer);

int main(void)
{
    socket_factory_t * factory = factory_init("127.0.0.1", "44567");
    if (server_connect(factory, TCP) == 0) 
    {
        // Did not connect correctly
        return -1;
    }
    int socket_fd = factory_get_socket(factory);
    char * buffer = calloc(sizeof(char), BUFFSIZE);
    while (1)
    {
        COMMAND command = get_command(buffer);
        
        write(socket_fd, &command, sizeof(command));
        
        switch (command)
        {
            case QUIT:
                free(buffer);
                factory_destroy(factory);
                return 0;
            case LIST:
                read_listing(socket_fd, buffer);
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

unsigned int get_command(char * buffer)
{
    unsigned int choice = 0;
    while (1)
    {
        memset(buffer, 0, BUFFSIZE);
        fputs("ftp> ", stdout);
        fgets(buffer, BUFFSIZE, stdin);
        
        if (strcmp(buffer, "quit\n") == 0)
        {
            choice = QUIT;
            break;
        }
        else if (strcmp(buffer, "ls\n") == 0)
        {
            choice = LIST;
            break;
        }
        else
        {
            fprintf(stderr, "Unknown command: %s", buffer);
        }
    }
    
    return choice;
}

void read_listing(int socket_fd, char * buffer)
{
    do
    {
        // Reset buffer to read from socket
        memset(buffer, 0, BUFFSIZE);
        read(socket_fd, buffer, BUFFSIZE);
        printf("%s", buffer);
    } while (strchr(buffer, 0x4) == NULL);
    
    fputs("\n", stdout);
}
// END OF SOURCE
