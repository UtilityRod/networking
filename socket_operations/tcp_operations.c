#include <tcp_operations.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

int tcp_read_all(int socket_fd, void * buffer, size_t buffer_sz)
{
    if ((socket_fd < 0) || (NULL == buffer) || (0 == buffer_sz))
    {
        return -1;
    }

    uint8_t * internal_buffer = (uint8_t *)buffer;
    int amount_read = 0;
    int rv = 0;
    while (amount_read < buffer_sz)
    {
        char * read_buffer = internal_buffer + amount_read;
        int read_size = buffer_sz - amount_read;
        int tmp_read = read(socket_fd, read_buffer, read_size);

        if (-1 == tmp_read)
        {
            rv = -1;
            break;
        }
        else
        {
            amount_read += tmp_read;
        }
    }

    return (rv != -1) ? amount_read : rv;
}

int tcp_send_all(int socket_fd, void * buffer, size_t buffer_sz)
{
    if (socket_fd <= 0)
    {
        return -1;
    }

    uint8_t * internal_buffer = (uint8_t *)buffer;
    size_t amount_sent = 0;
    int rv = 0;
    while (amount_sent < buffer_sz)
    {
        char * write_data = internal_buffer + amount_sent;
        size_t write_sz = buffer_sz - amount_sent;
        int tmp_sent = write(socket_fd, write_data, write_sz);

        if (tmp_sent == -1)
        {
            rv = -1;
            break;
        }
        else
        {
            amount_sent += tmp_sent;
        }
    }

    return (rv != -1) ? amount_sent : rv;
}
// END OF SOURCE