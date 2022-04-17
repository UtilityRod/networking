#include <uds_server.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>

struct uds_server
{
    char * socket_file;
    int socket_fd;
};

enum {SUN_PATH_SZ = 108, BACKLOG = 10};

static int setup_server(uds_server_t * server);

uds_server_t * uds_server_setup(char * socket_file)
{
    
    if (NULL == socket_file)
    {
        return NULL;
    }

    // Check to see if the socket file passed exists
    struct stat statbuff = {0};
    int stat_check = stat(socket_file, &statbuff);

    if (stat_check != -1)
    {
        // If file exists and it is a socket file then unlink
        if (S_ISSOCK(statbuff.st_mode))
        {
            unlink(socket_file);
        }
        else
        {
            // File exists and is not a socket file, return NULL back to calling function
            fprintf(stderr, "File %s exists and is not a socket file.\n", socket_file);
            return NULL;
        }
    }

    uds_server_t * server = calloc(1, sizeof(*server));

    if (NULL == server)
    {
        perror("UDS calloc");
        return NULL;
    }

    server->socket_file = socket_file;
    server->socket_fd = setup_server(server);

    if (-1 == server->socket_fd)
    {
        fprintf(stderr, "Unix Domain Socket server failed to setup.\n");
        free(server);
        server = NULL;
    }

    return server;
}

void uds_server_teardown(uds_server_t * server)
{
    if (server != NULL)
    {
        if (server->socket_fd != -1)
        {
            // Close socket file descriptor if still active
            close(server->socket_fd);
            server->socket_fd = -1;
        }

        free(server);
    }
}

int uds_server_accept(uds_server_t * server)
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

static int setup_server(uds_server_t * server)
{
    // Create socket
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (-1 == socket_fd)
    {
        perror("UDS socket");
        return -1;
    }
    // Set up sockaddr_un for bind
    struct sockaddr_un server_addr = {
                            .sun_family = AF_UNIX
                        };
    strncpy(server_addr.sun_path, server->socket_file, SUN_PATH_SZ - 1);
    int addr_len = strlen(server_addr.sun_path) + sizeof(server_addr.sun_family);
    int bind_check = bind(socket_fd, (struct sockaddr *)&server_addr, addr_len);

    if (-1 == bind_check)
    {
        perror("UDS bind");
        close(socket_fd);
        return -1;
    }

    // Listen on the socket using the backlog
    int listen_check = listen(socket_fd, BACKLOG);

    if (-1 == listen_check)
    {
        perror("UDS listen");
        close(socket_fd);
        return -1;
    }

    return socket_fd;
}
// END OF SOURCE