#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <socket_factory.h>
#include <regex.h>

enum {BUFFSIZE = 512};

static char * print_buffer(const char * msg);

int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        perror("Socket file descriptor not found.");
        return -1;
    }
    
    socket_factory_t * factory = factory_init(NULL, NULL);
    factory_set_socket(factory, atoi(argv[1]));
    if(tcp_recv_msg(factory) != 0)
    {
        perror("Message receive.");
        return -1;
    }
    
    char * filepath = handle_buffer(factory, print_buffer);
    printf("Filepath: %s\n", filepath);
    free(filepath);
}

static char * print_buffer(const char * msg)
{
    regex_t regex;
    int flags = 0;
    regcomp(&regex, "^GET /.* HTTP/1\\.1\r\n$", flags);
    int value = regexec(&regex, msg, 0, NULL, 0);
    regfree(&regex);
    if (!value)
    {
        char * slash_index = strchr(msg, '/');
        char * space_index = strchr(slash_index, ' ');
        int filepath_sz = (space_index - slash_index) + 1;
        char * filepath = calloc(sizeof(char), filepath_sz);
        strncpy(filepath, slash_index, filepath_sz);
        return filepath;
    }

    return NULL;
}
