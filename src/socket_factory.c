#include "socket_factory.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>


struct socket_factory_ {
    const char * ip_addr;
    const char * port;
    int socket;
};

socket_factory_t * factory_init(const char * ip_addr, const char * port) 
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
        // Shutdown and close listening socket if it exists
        shutdown(factory->socket, SHUT_RDWR);
        close(factory->socket);
    }
    // Free memory for factory
    free(factory);
} 

int server_setup(socket_factory_t * factory) 
{
    // Struct to hold hints for server addr info
    struct addrinfo hints = {0};
    // Set the hints to proper values
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // Struct to hold the results of get addrinfo
    struct addrinfo * results;
    // Get the address info of the server
    int err = getaddrinfo(NULL, factory->port, &hints, &results);
    if (err != 0) 
    {
        // Could not get address info return back to calling function
        perror("Could not get address information.");
        return 0;
    }
    // Create socket using results from getaddrinfo
    int socket_fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if (socket_fd < 0)
    {
        // Socket not created properly return back to calling function
        perror("Socket creation.");
        freeaddrinfo(results);
        return 0;
    }
    
    // Bind socket to address using results from getaddrinfo
    if (bind(socket_fd, results->ai_addr, results->ai_addrlen) < 0)
    {
        // Socket bind failed return back to calling function
        perror("Socket Bind");
        freeaddrinfo(results);
        close(socket_fd);
        return 0;
    }
    // Free address info struct
    freeaddrinfo(results);
    // Listen on socket using newly created socket file descriptor
    if (listen(socket_fd, 0) < 0)
    {
        // Socket listen failed return back to calling function
        perror("Socket Listen");
        close(socket_fd);
        return 0;
    }
    // Set socket file descriptor to factory->socket
    factory->socket = socket_fd;
    // Return 1 to signal server was properly setup
    return 1;
}

void fork_listen(socket_factory_t * factory)
{
    // Structure that holds client information
    struct sockaddr_storage client;
    // Structure size
    socklen_t client_sz = sizeof(client);
    // Accept connections on socket
    int socket_connection = accept(factory->socket, (struct sockaddr *)&client, &client_sz);
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
    // Structure that holds server information
    struct addrinfo * results;
    // Get addr infor for server
    int err = getaddrinfo(factory->ip_addr, factory->port, NULL, &results);
    if (err != 0)
    {
        // Couldn't get information return back to calling program
        perror("Could not get address information.");
        return 0;
    }
    // Create socket to connect to server
    int socket_fd = socket(results->ai_family, results->ai_socktype, 0);
    if (socket_fd < 0)
    {
        // Socket creation failed return back to calling function
        perror("Socket create");
        freeaddrinfo(results);
        return 0;
    }
    // Connect to server using created socket and server information
    if(connect(socket_fd, results->ai_addr, results->ai_addrlen) < 0)
    {
        // Could not connect to socket return to calling function
        perror("Socket connect");
        freeaddrinfo(results);
        close(socket_fd);
        return 0;
    }
    // Free the addr info struct
    freeaddrinfo(results);
    // Assign the socket file descriptor to factory
    factory->socket = socket_fd;
    // Return 1 to signal that you are successfully connected
    return 1;
}

int factory_get_socket(socket_factory_t * factory)
{
    return factory->socket;
}
// END OF SOURCE
