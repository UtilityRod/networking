#include "tcp_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


struct tcp_server_
{
    const char * port; // String port
    int socket_fd; // The socket for communication
    struct addrinfo * server_info; // struct used for getaddrinfo()
};

typedef enum backlog {BACKLOG = 10} backlog_t;

static int setup_server(tcp_server_t * tcp_server);

tcp_server_t * tcp_server_setup(const char * port) 
{
    /*
     * Initialize the socket factory with default values and the values passed in
     * as arguments.
     */
    tcp_server_t * new = calloc(sizeof(*new), 1);
    
    if (new == NULL)
    {
        // calloc did not properly allocate memory
        perror("Error allocating memory for socket factory");
        return NULL;
    }
    else
    {
        // Set default values for struct data
        new->port = port;
        new->socket_fd = -1;
        new->server_info = NULL;
    }
    
    int setup_check = setup_server(new);
    if (setup_check != 0)
    {
        tcp_server_teardown(new);
        new = NULL;
    }
    
    return new;
}

void tcp_server_teardown(tcp_server_t * tcp_server) 
{
    /*
     * Free all the memory used in the socket factory
     */
    if (tcp_server->socket_fd != -1)
    {
        // Shutdown and close socket if it exists
        close(tcp_server->socket_fd);
    }
    // Free the struct used for getaddrinfo()
    if (tcp_server->server_info)
    {
        freeaddrinfo(tcp_server->server_info);
    }
    
    free(tcp_server);
}

int tcp_server_accept(tcp_server_t * tcp_server)
{
    // Accept on a socket that has been setup for the factory
    // Structure that holds client information
    struct sockaddr_storage client;
    // Structure size
    socklen_t client_sz = sizeof(client);
    // Accept connections on socket
    int connection = accept(tcp_server->socket_fd, (struct sockaddr *)&client, &client_sz);
    return connection;
}

void tcp_server_accept_fork(tcp_server_t * tcp_server, char * exe)
{
    /*
     * Function performs a signle accept on the factory's socket and will fork
     * a child process. The executable passing through dir_path will be passed
     * to execve as the new process.
     */
    int connection = tcp_server_accept(tcp_server);

    if (connection > 0)
    {
        // PID of parent/child from fork()
        pid_t pid = fork();
        // If pid is 0 it is the child. Execute binary
        if (pid == 0)
        {
            // Create argv (args) and envp for execve command
            char * args[] = {exe, '\0'};
            // Create character arrays for the environement variables needed for the ashti_handler
            char * env_socket= calloc(sizeof(char), 20); // The connection socket
            // Add correct values into the ENV strings
            snprintf(env_socket, 20, "SOCK=%d", connection);
            // Add all env variables to envp
            char * envp[] = {env_socket, '\0'};
            // Call execve on child to perform executable task
            execve(exe, args, envp);
        }
        else
        {
            close(connection);
        } 
    }
    
    // There is nothing to return
    return;
}

static int setup_server(tcp_server_t * tcp_server)
{
    /*
     * Sets up the hints struct for getaddrinfo, performs all the needed steps
     * to setup a socket for the TCP server, and then listens on that socket.
     */
    // Struct to hold hints for server addr info
    struct addrinfo hints = {
                                // Set the hints to proper values
                                .ai_family = AF_UNSPEC, // IPv6 or IPv4
                                .ai_socktype = SOCK_STREAM, // TCP
                                .ai_flags = AI_PASSIVE // Use current system's IP
                            };

    int err = getaddrinfo(NULL, tcp_server->port, &hints, &tcp_server->server_info);
    if (err != 0) 
    {
        // Could not get address info return back to calling function
        perror("Could not get address information.");
        return err;
    }
    // Loop through each result from getaddrinfo and find first one you can use
    struct addrinfo * p = NULL; // Iterator for the loop
    for (p = tcp_server->server_info; p != NULL; p = p->ai_next)
    {
        if ((tcp_server->socket_fd = socket(p->ai_family, p->ai_socktype, 
                                            p->ai_protocol)) == -1)
        {
            // Could not find address to bind to
            perror("TCP Server Socket Create.");
            continue;
        }
        int tmp = 1;
        if (setsockopt(tcp_server->socket_fd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int)) == -1)
        {
            // Could not correct set socket options
            perror("TCP Socket Options");
            return -1;
        }

        if (bind(tcp_server->socket_fd, p->ai_addr, p->ai_addrlen) == -1)
        {
            // Could not bind socket
            close (tcp_server->socket_fd);
            tcp_server->socket_fd = -1;
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fputs("Server setup failed.\n", stderr);
        return -2;
    }

    if (listen(tcp_server->socket_fd, BACKLOG) == -1)
    {
        perror("TCP Server Listen.");
        return -3;
    }

    return 0;
}
// END OF SOURCE