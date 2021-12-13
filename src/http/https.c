#include <stdio.h>
#include <socket_factory.h>
#include <stdlib.h>
#include <unistd.h>

enum {BUFFSIZE = 512};

int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        perror("Socket file descriptor not found.");
        return -1;
    }
    
    int socket_fd = atoi(argv[1]);
    char * buffer = calloc(sizeof(char), BUFFSIZE);
    int amt_read = read(socket_fd, buffer, BUFFSIZE - 1);
    buffer[amt_read] = '\0';
    
    printf("Data read: %s\n", buffer);
    free(buffer);
}
