#include <stdio.h>
#include <stdlib.h>
#include <socket_factory.h>

enum {BUFFSIZE = 512};

static void print_buffer(const char * msg);

int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        perror("Socket file descriptor not found.");
        return -1;
    }
    
    socket_factory_t * factory = factory_init(NULL, NULL);
    factory_set_socket(factory, atoi(argv[1]));
    if(tcp_recv_msg(factory) == 0)
    {
        handle_buffer(factory, print_buffer);
    }
}

static void print_buffer(const char * msg)
{
    printf("Data read: %s\n", msg);
}
