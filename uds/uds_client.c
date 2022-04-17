#include <uds_client.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>

struct uds_client
{
    int socket_fd;
    char * socket_file;
};

enum {SUN_PATH_SZ = 108, BACKLOG = 10};

static int setup_client(uds_client_t * server);

uds_client_t * uds_client_setup(char * socket_file)
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
        // If file exists and it is not a socket file then return NULL
        if (!S_ISSOCK(statbuff.st_mode))
        {
            fprintf(stderr, "File %s is not a valid Unix Domain Socket file.\n", socket_file);
            return NULL;
        }
    }

    uds_client_t * client = calloc(1, sizeof(*client));

    if (NULL == client)
    {
        perror("UDS client calloc");
        return NULL;
    }

    client->socket_file = socket_file;
    client->socket_fd = setup_client(client);

    if (-1 == client->socket_fd)
    {
        fprintf(stderr, "Unix Domain Socket client failed to setup.\n");
        free(client);
        client = NULL;
    }

    return client;
}

void uds_client_teardown(uds_client_t * client)
{
    if (client != NULL)
    {
        if (client->socket_fd != -1)
        {
            // Close socket file descriptor if still active
            close(client->socket_fd);
            client->socket_fd = -1;
        }

        free(client);
    }
}

int uds_client_get_socket(uds_client_t * client)
{
    return (client != NULL) ? client->socket_fd : -1;
}

static int setup_client(uds_client_t * server)
{
    // Create socket
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (-1 == socket_fd)
    {
        perror("UDS client socket");
        return -1;
    }
    // Set up sockaddr_un for bind
    struct sockaddr_un server_addr = {
                            .sun_family = AF_UNIX
                        };
    strncpy(server_addr.sun_path, server->socket_file, SUN_PATH_SZ - 1);
    int addr_len = strlen(server_addr.sun_path) + sizeof(server_addr.sun_family);
    int connect_check = connect(socket_fd, (struct sockaddr *)&server_addr, addr_len);

    if (-1 == connect_check)
    {
        perror("UDS client connect");
        close(socket_fd);
        socket_fd = -1;
    }

    return socket_fd;
}
// END OF SOURCE