#include "tcp_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ssl_operations.h>

typedef enum {HOSTNAME_SZ = 50} network_sizes_t;

struct tcp_server_
{
    const char * port; // String port
    int socket_fd; // The socket for communication
    struct addrinfo * server_info; // struct used for getaddrinfo()
    char hostname[HOSTNAME_SZ];
};

typedef enum backlog {BACKLOG = 10} backlog_t;

static int setup_server(tcp_server_t * tcp_server);

tcp_server_t * tcp_server_setup(const char * port) 
{
    /*
     * Initialize the socket factory with default values and the values passed in
     * as arguments.
     */
    tcp_server_t * server = calloc(sizeof(*server), 1);
    
    if (server == NULL)
    {
        // calloc did not properly allocate memory
        perror("Error allocating memory for socket factory");
        return NULL;
    }
    else
    {
        // Set default values for struct data
        server->port = port;
        server->socket_fd = -1;
        server->server_info = NULL;
        gethostname(server->hostname, HOSTNAME_SZ);
        printf("Hostname %s\n", server->hostname);
    }
    
    int setup_check = setup_server(server);
    if (setup_check != 0)
    {
        tcp_server_teardown(server);
        server = NULL;
    }
    
    return server;
}

void tcp_server_teardown(tcp_server_t * server) 
{
    /*
     * Free all the memory used in the socket factory
     */
    if (server->socket_fd != -1)
    {
        // Shutdown and close socket if it exists
        close(server->socket_fd);
    }
    // Free the struct used for getaddrinfo()
    if (server->server_info != NULL)
    {
        freeaddrinfo(server->server_info);
    }
    
    free(server);
}

int tcp_server_accept(tcp_server_t * server)
{
    // Accept on a socket that has been setup for the factory
    // Structure that holds client information
    struct sockaddr_storage client;
    // Structure size
    socklen_t client_sz = sizeof(client);
    // Accept connections on socket
    int connection = accept(server->socket_fd, (struct sockaddr *)&client, &client_sz);
    return connection;
}

void tcp_server_accept_fork(tcp_server_t * server, char * exe)
{
    /*
     * Function performs a signle accept on the factory's socket and will fork
     * a child process. The executable passing through dir_path will be passed
     * to execve as the server process.
     */
    int connection = tcp_server_accept(server);

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

int tcp_server_get_socket(tcp_server_t * server)
{

    return (server != NULL) ? server->socket_fd : -1;
}

static int setup_server(tcp_server_t * server)
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

    int err = getaddrinfo(NULL, server->port, &hints, &server->server_info);
    if (err != 0) 
    {
        // Could not get address info return back to calling function
        perror("Could not get address information.");
        return err;
    }
    // Loop through each result from getaddrinfo and find first one you can use
    struct addrinfo * p = NULL; // Iterator for the loop
    for (p = server->server_info; p != NULL; p = p->ai_next)
    {
        server->socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (-1 == server->socket_fd)
        {
            // Could not find address to bind to
            perror("TCP server: socket()");
            continue;
        }
        int tmp = 1;
        int option_check = setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int));
        if (-1 == option_check)
        {
            // Could not correct set socket options
            close (server->socket_fd);
            server->socket_fd = -1;
            perror("TCP server: setsockopt()");
            return -1;
        }

        int bind_check = bind(server->socket_fd, p->ai_addr, p->ai_addrlen);
        if (-1 == bind_check)
        {
            // Could not bind socket
            close (server->socket_fd);
            server->socket_fd = -1;
            perror("TCP server: bind()");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fputs("Server setup failed.\n", stderr);
        return -2;
    }

    if (listen(server->socket_fd, BACKLOG) == -1)
    {
        perror("TCP Server: listen()");
        return -3;
    }

    return 0;
}
// END OF SOURCE