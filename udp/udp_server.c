#include <udp_server.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stddef.h>


struct udp_server
{
    const char * port; // String port
    int socket_fd; // The socket for communication
    struct addrinfo * server_info; // struct used for getaddrinfo()
};

static int setup_server(udp_server_t * server);

udp_server_t * udp_server_setup(char * port)
{
    if (NULL == port)
    {
        // Port must not be NULL
        return NULL;
    }

    udp_server_t * server = calloc(1, sizeof(*server));

    if (server != NULL)
    {
        // Memory allocation for server was successfull
        server->port = port;
        server->socket_fd = -1;


        // Internal server setup
        int tmp_socket_fd = setup_server(server);

        if (tmp_socket_fd <= 0)
        {
            // Internal server setup failed.
            udp_server_teardown(server);
            server = NULL;
        }
        else
        {
            // Internal server setup successful, update structure socker_fd
            server->socket_fd = tmp_socket_fd;
        }
    }

    return server;
}

void udp_server_teardown(udp_server_t * server) 
{
    /*
     * Free all the memory used in the udp server
     */
    if (server->socket_fd != -1)
    {
        // Shutdown and close socket if it exists
        close(server->socket_fd);
        server->socket_fd = -1;
    }
    // Free the struct used for getaddrinfo()
    if (server->server_info)
    {
        freeaddrinfo(server->server_info);
    }
    
    free(server);
}

ssize_t udp_server_recv_all(udp_server_t * server, char * buffer, size_t max)
{
    struct sockaddr_storage client_addr = {0};
    socklen_t addr_len = sizeof(client_addr);
    // Read all data from UDP socket
    ssize_t amount_read = recvfrom(server->socket_fd, buffer, max - 1 , 0, (struct sockaddr *)&client_addr, &addr_len);
    return amount_read;
}

static int setup_server(udp_server_t * server)
{
    struct addrinfo hints = {
                                .ai_family = AF_UNSPEC, // IPv4 of IPv6
                                .ai_socktype = SOCK_DGRAM, // UDP
                                .ai_flags = AI_PASSIVE
                            };


    int err = getaddrinfo(NULL, server->port, &hints, &(server->server_info));
    if (err != 0)
    {
        // Could not get address info return back to calling function
        perror("UDP server getaddrinfo.");
        return err;
    }

    // Loop through all addresses and get first one
    struct addrinfo * p = NULL; // Iterator for the loop
    int socket_fd = -1;
    for (p = server->server_info; p != NULL; p = p->ai_next)
    {
        // Create socket
        socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (-1 == socket_fd)
        {
            perror("UDP server socket.");
            continue;
        }
        // Bind to socket
        int bind_check = bind(socket_fd, p->ai_addr, p->ai_addrlen);
        if (-1 == bind_check)
        {
            close(socket_fd);
            perror("UDP server bind.");
            continue;
        }

        break;
    }

    if (NULL == p)
    {
        fprintf(stderr, "Server failed to bind on socket.\n");
        return -1;
    }

    return socket_fd;
}
// END OF SOURCE