#include <socket_factory.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


struct socket_factory_ 
{
    const char * p_port; // String port
    int socket_h; // The socket for communication
    struct addrinfo * p_server_info; // struct used for getaddrinfo()
};

typedef enum backlog_ {BACKLOG = 10} backlog_t;

socket_factory_t * factory_create(const char * p_port) 
{
    /*
     * Initialize the socket factory with default values and the values passed in
     * as arguments.
     */
    socket_factory_t * p_new = calloc(sizeof(*p_new), 1);
    
    if (p_new == NULL)
    {
        // malloc did not properly allocate memory
        perror("Error allocating memory for socket factory");
        return NULL;
    }
    else
    {
        // Set default values for struct data
        p_new->p_port = p_port;
        p_new->socket_h = -1;
        p_new->p_server_info = NULL;
    }
    
    
    return p_new;
}

void factory_destroy(socket_factory_t * p_factory) 
{
    /*
     * Free all the memory used in the socket factory
     */
    if (p_factory->socket_h != -1)
    {
        // Shutdown and close socket if it exists
        close(p_factory->socket_h);
    }
    // Free the struct used for getaddrinfo()
    if (p_factory->p_server_info)
    {
        freeaddrinfo(p_factory->p_server_info);
    }
    
    free(p_factory);
}

int tcp_server_setup(socket_factory_t * p_factory)
{
    /*
     * Sets up the hints struct for getaddrinfo, performs all the needed steps
     * to setup a socket for the TCP server, and then listens on that socket.
     */
    // Struct to hold hints for server addr info
    struct addrinfo hints = {0};
    // Set the hints to proper values
    hints.ai_family = AF_UNSPEC; // IPv6 or IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE; // Use current system's IP

    int err = getaddrinfo(NULL, p_factory->p_port, &hints, &p_factory->p_server_info);
    if (err != 0) 
    {
        // Could not get address info return back to calling function
        perror("Could not get address information.");
        return err;
    }
    // Loop through each result from getaddrinfo and find first one you can use
    struct addrinfo * p = NULL; // Iterator for the loop
    for (p = p_factory->p_server_info; p != NULL; p = p->ai_next)
    {
        if ((p_factory->socket_h = socket(p->ai_family, p->ai_socktype, 
                                            p->ai_protocol)) == -1)
        {
            // Could not find address to bind to
            perror("TCP Server Socket Create.");
            continue;
        }
        int tmp = 1;
        if (setsockopt(p_factory->socket_h, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int)) == -1)
        {
            // Could not correct set socket options
            perror("TCP Socket Options");
            exit(-1);
        }

        if (bind(p_factory->socket_h, p->ai_addr, p->ai_addrlen) == -1)
        {
            // Could not bind socket
            close (p_factory->socket_h);
            p_factory->socket_h = -1;
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fputs("Server setup failed.\n", stderr);
        exit(-2);
    }

    if (listen(p_factory->socket_h, BACKLOG) == -1)
    {
        perror("TCP Server Listen.");
    }

    return 0;
}

void tcp_accept_fork(socket_factory_t * p_factory, const char * p_exe)
{
    /*
     * Function performs a signle accept on the factory's socket and will fork
     * a child process. The executable passing through dir_path will be passed
     * to execve as the new process.
     */
    // Structure that holds client information
    struct sockaddr_storage client;
    // Structure size
    socklen_t client_sz = sizeof(client);
    // Accept connections on socket
    int connection = accept(p_factory->socket_h, (struct sockaddr *)&client, &client_sz);
    if (connection < 0)
    {
        // Accept failed so return
        return;
    }

    // PID of parent/child from fork()
    pid_t pid = fork();
    // If pid is 0 it is the child. Execute binary
    if (pid == 0)
    {
        // Create argv (args) and envp for execve command
        char * p_args[] = {p_exe, '\0'};
        // Create character arrays for the environement variables needed for the ashti_handler
        char * p_env_socket= calloc(sizeof(char), 20); // The connection socket
        // Add correct values into the ENV strings
        snprintf(p_env_socket, 20, "SOCK=%d", connection);
        // Add all env variables to envp
        char * p_envp[] = {p_env_socket, '\0'};
        // Call execve on child to perform executable task
        execve(p_exe, p_args, p_envp);
    }
    else
    {
        close(connection);
    }
    // There is nothing to return
    return;
}

// END OF SOURCE