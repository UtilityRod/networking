#include "socket_factory.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>


struct socket_factory_ {
    const char * ip_addr;
    unsigned int port;
    int socket;
};

socket_factory_t * factory_init(const char * ip_addr, unsigned int port) 
{
    socket_factory_t * new_factory = malloc(sizeof(socket_factory_t));
    new_factory->ip_addr = ip_addr;
    new_factory->port = port;
    new_factory->socket = -1;
    return new_factory;
}

void factory_destroy(socket_factory_t * factory) 
{
    if (factory->socket != -1)
    {
        shutdown(factory->socket, SHUT_RDWR);
        close(factory->socket);
    }
    
    free(factory);
} 

int server_setup(socket_factory_t * factory)
{
    // Create TCP socket using AF_INET family
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        // Socket not created properly
        perror("Socket creation.");
        return 0;
    }
    
    // Assign server address information to struct
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(factory->port);
    server_addr.sin_addr.s_addr = inet_addr(factory->ip_addr);
    
    // Bind socket to address
    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        // Socket bind failed
        perror("Socket Bind");
        close(socket_fd);
        return 0;
    }
    
    // Listen on socket
    if (listen(socket_fd, 0) < 0)
    {
        // Socket listen failed
        perror("Socket Listen");
        close(socket_fd);
        return 0;
    }
    
    // Return socket_fd for the connection handler
    factory->socket = socket_fd;
    return 1;
}

void fork_listen(socket_factory_t * factory)
{
    // Accept connection on socket
    struct sockaddr_in client_addr;
    unsigned int client_sz = sizeof(client_addr);
    // socket_connection -> file descriptor for connected client
    int socket_connection = accept(factory->socket, (struct sockaddr *)&client_addr, &client_sz);
    if (socket_connection < 0)
    {
        // Accept failed so return
        return;
    }
    
    // PID of parent/child from fork()
    pid_t pid = fork();
    // If pid is 0 it is the child. Execute binary
    if (pid == 0)
    {
        // Binary executable path
        char executable[] = "./ftps";
        // Socket file descriptor string for cmd line arguments
        char * sock_str = malloc(sizeof(char) * 10);
        // Put socket connection FD into string
        sprintf(sock_str, "%d", socket_connection);
        // Create argv (args) and envp for execve command
        // Command line arguments:
        // 1. Executable name
        // 2. Socket connection file descriptor
        char * args[] = {executable, sock_str, '\0'};
        char * envp[] = {'\0'};
        // Call execve on child to perform executable task
        execve(executable, args, envp);
    }
    // There is nothing to return
    return;
}

int server_connect(socket_factory_t * factory)
{
    // Create socket file descriptor
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (socket_fd < 0)
    {
        perror("Socket create");
        return 0;
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(factory->port);
    server_addr.sin_addr.s_addr = inet_addr(factory->ip_addr);
    
    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Socket connect");
        close(socket_fd);
        return 0;
    }
    
    factory->socket = socket_fd;
    return 1;
}

int factory_get_socket(socket_factory_t * factory)
{
    return factory->socket;
}
// END OF SOURCE
