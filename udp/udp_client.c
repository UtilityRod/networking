#include <udp_client.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

struct udp_client
{
    const char * port; // String port
    const char * server;
    int socket_fd; // The socket for communication
    struct addrinfo * server_info; // struct used for getaddrinfo()
};

static int setup_client(udp_client_t * client);

udp_client_t * udp_client_setup(char * server, char * port)
{
    if (NULL == port)
    {
        // Port must not be NULL
        return NULL;
    }

    udp_client_t * client = calloc(1, sizeof(*client));

    if (client != NULL)
    {
        // Memory allocation for client was successfull
        client->port = port;
        client->server = server;
        client->socket_fd = -1;

        // Internal client setup
        int tmp_socket_fd = setup_client(client);

        if (tmp_socket_fd <= 0)
        {
            // Interal client setup failed
            udp_client_teardown(client);
            client = NULL;
        }
        else
        {
            // Internal client setup successfull, update structure socket_fd
            client->socket_fd = tmp_socket_fd;
        }
    }

    return client;
}

void udp_client_teardown(udp_client_t * client) 
{
    /*
     * Free all the memory used in the udp client
     */
    if (client->socket_fd != -1)
    {
        // Shutdown and close socket if it exists
        close(client->socket_fd);
        client->socket_fd = -1;
    }
    // Free the struct used for getaddrinfo()
    if (client->server_info)
    {
        freeaddrinfo(client->server_info);
    }
    
    free(client);
}

ssize_t udp_client_send_mssage(udp_client_t * client, char * buffer, size_t size)
{
    struct addrinfo * server_info = client->server_info;
    ssize_t amount_sent = 0;
    while (amount_sent != size)
    {
        char * offset = buffer + amount_sent;
        ssize_t tmp_read = sendto(client->socket_fd, offset, strlen(offset), 0, server_info->ai_addr, server_info->ai_addrlen);

        if (-1 == tmp_read)
        {
            // Error of read
            perror("Client sendto");
            break;
        }
        else
        {
            // Partial write, update amount_sent and redo while loop
            amount_sent += tmp_read;
        }
    }
    
    return amount_sent;
}

static int setup_client(udp_client_t * client)
{
    struct addrinfo hints = {
                                .ai_family = AF_UNSPEC,
                                .ai_socktype = SOCK_DGRAM
                            };


    int err = getaddrinfo(client->server, client->port, &hints, &(client->server_info));
    if (err != 0)
    {
        // Could not get address info return back to calling function
        perror("UDP client getaddrinfo.");
        return err;
    }

    // Loop through all addresses and get first one
    struct addrinfo * p = NULL; // Iterator for the loop
    int socket_fd = -1;
    for (p = client->server_info; p != NULL; p = p->ai_next)
    {
        // Create socket
        socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (-1 == socket_fd)
        {
            perror("UDP client socket.");
            continue;
        }

        break;
    }

    if (NULL == p)
    {
        fprintf(stderr, "Client failed to connect on socket.\n");
        return -1;
    }
    else
    {
        client->server_info = p;
    }

    return socket_fd;
}
// END OF SOURCE