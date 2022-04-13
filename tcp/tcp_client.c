#include <tcp_client.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

struct tcp_client
{
    const char * port; // String port
    const char * address;
    int socket_fd; // The socket for communication
    struct addrinfo * server_info; // struct used for getaddrinfo()
};

tcp_client_t * tcp_client_setup(const char * address, const char * port) 
{
    /*
     * Initialize the tcp client with default values and the values passed in
     * as arguments.
     */
    tcp_client_t * client = calloc(sizeof(*client), 1);
    
    if (client == NULL)
    {
        // calloc did not properly allocate memory
        perror("Error allocating memory for socket factory");
        return NULL;
    }
    else
    {
        // Set default values for struct data
        client->address = address;
        client->port = port;
        client->socket_fd = -1;
        client->server_info = NULL;
    }
    
    return client;
}

void tcp_client_teardown(tcp_client_t * client) 
{
    tcp_client_disconnect(client);
    free(client);
}

int tcp_client_connect(tcp_client_t * client)
{
    /*
     * Sets up the hints struct for getaddrinfo, performs all the needed steps
     * to setup a socket for the TCP client, and then listens on that socket.
     */
    // Struct to hold hints for client addr info
    struct addrinfo hints = {
                                // Set the hints to proper values
                                .ai_family = AF_UNSPEC, // IPv6 or IPv4
                                .ai_socktype = SOCK_STREAM, // TCP
                            };

    int err = getaddrinfo(client->address, client->port, &hints, &client->server_info);
    if (err != 0) 
    {
        // Could not get address info return back to calling function
        perror("TCP Client: getaddrinfo.");
        return err;
    }

    // Loop through each result from getaddrinfo and find first one you can use
    struct addrinfo * p = NULL; // Iterator for the loop
    for (p = client->server_info; p != NULL; p = p->ai_next)
    {
        client->socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (client->socket_fd == -1)
        {
            // Could not find address to bind to
            perror("TCP client: socket");
            continue;
        }

        int connect_check = connect(client->socket_fd, p->ai_addr, p->ai_addrlen);
        if (-1 == connect_check)
        {
            close(client->socket_fd);
            client->socket_fd = -1;
            perror("TCP client: connect");
            continue;
        }

        break;
    }

    if (NULL == p)
    {
        fputs("Client could not connect to server.\n", stderr);
        return -1;
    }

    return 0;
}

void tcp_client_disconnect(tcp_client_t * client)
{
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
        client->server_info = NULL;
    }
}
// END OF SOURCE