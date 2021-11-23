#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

enum {BUFFSIZE = 512};
typedef enum {QUIT, LIST, PULL, PUSH} COMMAND;

int server_connect(const char * addr, unsigned int port);
unsigned int get_command(char * buffer);
void read_listing(int socket_fd, char * buffer);

int main(void)
{
    int socket_fd = server_connect("127.0.0.1", 44567);
    char * buffer = calloc(sizeof(char), BUFFSIZE);
    while (1)
    {
        COMMAND command = get_command(buffer);
        
        write(socket_fd, &command, sizeof(command));
        
        switch (command)
        {
            case QUIT:
                free(buffer);
                close(socket_fd);
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

int server_connect(const char * addr, unsigned int port)
{
    // Create socket file descriptor
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (socket_fd < 0)
    {
        perror("Socket create");
        return -1;
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(addr);
    
    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Socket connect");
        close(socket_fd);
        return -1;
    }
    
    return socket_fd;
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
