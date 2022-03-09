#include <fork_main.h>

int main(void)
{
    char * connection = getenv("SOCK");
    printf("New connection on socket file descriptor: %s\n", connection);
    int socket_h = atoi(connection);
    close(socket_h);
}