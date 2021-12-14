#include "socket_factory.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

static int server_setup(socket_factory_t * factory, struct addrinfo hints);

enum {BUFFSIZE = 512};

struct socket_factory_ {
    const char * ip_addr; // String IP address
    const char * port; // String port
    char * buffer; // Buffer for server/client communication
    int socket; // The socket for communication
    struct  addrinfo * results; // struct used for getaddrinfo()
};

socket_factory_t * factory_init(const char * ip_addr, const char * port) 
{
    socket_factory_t * new_factory = malloc(sizeof(socket_factory_t));
    new_factory->ip_addr = ip_addr;
    new_factory->port = port;
    new_factory->socket = -1;
    new_factory->buffer = malloc(sizeof(char) * BUFFSIZE);
    memset(new_factory->buffer, 0, BUFFSIZE);
    return new_factory;
}

void factory_destroy(socket_factory_t * factory) 
{
    if (factory->socket != -1)
    {
        // Shutdown and close listening socket if it exists
        close(factory->socket);
    }
    // Free the struct used for getaddrinfo()
    freeaddrinfo(factory->results);
    // Free memory for factory
    free(factory->buffer);
    free(factory);
}

int factory_get_socket(socket_factory_t * factory)
{
    return factory->socket;
}

void factory_set_socket(socket_factory_t * factory, int socket_fd)
{
    factory->socket = socket_fd;
} 

char * handle_buffer(socket_factory_t * factory, char * (* function)(const char * msg))
{
    return function(factory->buffer);
}

int server_connect(socket_factory_t * factory, socket_type type)
{
    // Get addr infor for server
    int err = getaddrinfo(factory->ip_addr, factory->port, NULL, &factory->results);
    if (err != 0)
    {
        // Couldn't get information return back to calling program
        perror("Could not get address information.");
        return 0;
    }
    // Check to see if you need to use TCP or UDP socket type
    int sock_type = (type == TCP) ? factory->results->ai_socktype : SOCK_DGRAM;
    // Create socket to communication with server
    int socket_fd = socket(factory->results->ai_family, sock_type, 0);
    if (socket_fd < 0)
    {
        // Socket creation failed return back to calling function
        perror("Socket create");
        return 0;
    }
    // Connect to server using created socket and server information
    if(type == TCP && connect(socket_fd, factory->results->ai_addr, factory->results->ai_addrlen) < 0)
    {
        // Could not connect to socket return to calling function
        perror("Socket connect");
        close(socket_fd);
        return 0;
    }
    // Assign the socket file descriptor to factory
    factory->socket = socket_fd;
    // Return 1 to signal that you are successfully connected
    return 1;
}

char * udp_recv_msg(socket_factory_t * factory)
{
    // Struct to hold the clients address information
    struct sockaddr_storage client;
    socklen_t client_sz = sizeof(client);
    // Zero out buffer for clean data read
    memset(factory->buffer, 0, BUFFSIZE);
    // Block on recvfrom until client sends data
    ssize_t received = recvfrom(factory->socket, factory->buffer, BUFFSIZE - 1,
                                                    0, (struct sockaddr *)&client, &client_sz);
    // Check to see if data was received
    if (received < 0)
    {
        perror("Error trying to receive from.");
        return 0;
    }
    // Append null terminating character at the end of the string to ensure length
    // is less than BUFFSIZE
    factory->buffer[received] = '\0';
    // Return the pointer to the buffer back to the calling function
    return factory->buffer;
}

int udp_send_msg(socket_factory_t * factory, char * msg) 
{
    // Send the message using the factory's results struct
    int sent = sendto(factory->socket, msg, strlen(msg), 0,
                                factory->results->ai_addr, factory->results->ai_addrlen);
    // Quick check to see if data was sent
    if (sent < 0)
    {
        perror("Did not send any data");
        return 0;
    }
    else
    {
        printf("Amount sent: %d\n", sent);
    }
    
    return 1;
}

int tcp_server_setup(socket_factory_t * factory)
{
    // Struct to hold hints for server addr info
    struct addrinfo hints = {0};
    // Set the hints to proper values
    hints.ai_family = AF_INET; // IPv6 or IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    if (!server_setup(factory, hints)) 
    {
        return 0;
    }
    // Listen on socket using newly created socket file descriptor
    if (listen(factory->socket, 0) < 0)
    {
        // Socket listen failed return back to calling function
        perror("Socket Listen");
        close(factory->socket);
        return 0;
    }

    return 1;
}

void tcp_fork_listen(socket_factory_t * factory, char * exe)
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
        // Socket file descriptor string for cmd line arguments
        char * sock_str = malloc(sizeof(char) * 10);
        // Put socket connection FD into string
        sprintf(sock_str, "%d", socket_connection);
        // Create argv (args) and envp for execve command
        // Command line arguments:
        // 1. Executable name
        // 2. Socket connection file descriptor
        char * args[] = {exe, sock_str, '\0'};
        char * envp[] = {'\0'};
        // Call execve on child to perform executable task
        execve(exe, args, envp);
    }
    // There is nothing to return
    return;
}

int tcp_accept(socket_factory_t * factory)
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
        return -1;
    }
    // Zero out buffer to ensure clean read
    memset(factory->buffer, 0, BUFFSIZE);
    // Read data from the socket
    int amt_read = read(socket_connection, factory->buffer, BUFFSIZE - 1);
    // If no data is read send error and return
    if (amt_read <= 0)
    {
        perror("Could not read any data");
        return -1;
    }
    // Print out data read from socket
    printf("Read data from client: %s\n", factory->buffer);
    return 0;
}

int tcp_send_msg(socket_factory_t * factory, const char * msg)
{
    int amt_sent = write(factory->socket, msg, strlen(msg));
    if (amt_sent <= 0)
    {
        perror("Could not send data");
        return -1;
    }
    return 0;
}

int tcp_recv_msg(socket_factory_t * factory)
{
    memset(factory->buffer, 0, BUFFSIZE);
    int read_amt = read(factory->socket, factory->buffer, BUFFSIZE - 1);
    if (read_amt <= 0)
    {
        perror("Could not read data");
        return -1;
    }
    factory->buffer[BUFFSIZE - 1] = '\0';
    return 0;
}

int udp_server_setup(socket_factory_t * factory)
{
    // Setup hints for udp server
    struct addrinfo hints = {0};
    hints.ai_family = PF_UNSPEC; // IPv6 or IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP
    hints.ai_flags = AI_PASSIVE;
    // No need to listen due to UDP, so just pass return value from server setup
    return server_setup(factory, hints);
}

static int server_setup(socket_factory_t * factory, struct addrinfo hints) 
{
    // Get the address info of the server
    int err = getaddrinfo(NULL, factory->port, &hints, &factory->results);
    if (err != 0) 
    {
        // Could not get address info return back to calling function
        perror("Could not get address information.");
        return 0;
    }
    // Create socket using results from getaddrinfo
    int socket_fd = socket(factory->results->ai_family, factory->results->ai_socktype, factory->results->ai_protocol);
    if (socket_fd < 0)
    {
        // Socket not created properly return back to calling function
        perror("Socket creation.");
        return 0;
    }
    
    // Bind socket to address using results from getaddrinfo
    if (bind(socket_fd, factory->results->ai_addr, factory->results->ai_addrlen) < 0)
    {
        // Socket bind failed return back to calling function
        perror("Socket Bind");
        close(socket_fd);
        return 0;
    }
    // Set socket file descriptor to factory->socket
    factory->socket = socket_fd;
    // Return 1 to signal server was properly setup
    return 1;
}
// END OF SOURCE
