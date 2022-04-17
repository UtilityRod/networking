#include <udp_operations.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>

ssize_t udp_read_all(int socket_fd, char * buffer, 
                    size_t buffer_sz, responder_t respond)
{

    if ((socket_fd <= 0) || (NULL == buffer) || (buffer_sz < 1))
    {
        return -1;
    }

    struct sockaddr_storage dest_address;
    socklen_t addr_len = sizeof(dest_address);
    ssize_t amount_read = 0;
    while (amount_read < buffer_sz - 1)
    {
        // Read the expected amount of data into the buffer
        char * offset = buffer + amount_read;
        buffer_sz = buffer_sz - amount_read;
        ssize_t tmp_read = recvfrom(socket_fd, offset, buffer_sz , 0, 
                                    (struct sockaddr *)&dest_address, &addr_len);

        if (-1 == tmp_read)
        {
            // Error during read
            perror("UDP recvfrom");
            amount_read = -1;
            break;
        }
        else
        {
            // Did not read expected amount
            amount_read += tmp_read;
        }

    }

    if ((-1 != amount_read) && (respond != NULL))
    {
        // If all the data was read properly and the respond function is passed through
        // Call the respond function with the buffer
        int respond_check = respond(buffer, buffer_sz);

        if (0 == respond_check)
        {
            // Respond function return with 0 signifying the request was valid
            // Respond with the buffer that was set by the respond function.
            // Use the destination address that was populated during the recvfrom for the 
            // destination address.
            struct addrinfo tmp_info = {
                                    .ai_flags = AI_PASSIVE,
                                    .ai_socktype = SOCK_DGRAM,
                                    .ai_addr = (struct sockaddr *)&dest_address,
                                    .ai_addrlen = addr_len,
                                    .ai_family = dest_address.ss_family
                                    };
            // Call udp_send_all with addrinfo struct of destination
            udp_send_all(socket_fd, &tmp_info, buffer, strlen(buffer));
        }
    }
    // Return the amount read to the calling function
    return amount_read;
}


ssize_t udp_send_all(int socket_fd, struct addrinfo * info, char * buffer, size_t buffer_sz)
{
    if ((socket_fd <= 0) || (NULL == buffer) || (buffer_sz < 1))
    {
        return -1;
    }

    ssize_t amount_sent = 0;
    while (amount_sent < buffer_sz)
    {
        // Send all data passed to the function inside of the buffer to the server
        char * offset = buffer + amount_sent;
        buffer_sz = buffer_sz - amount_sent;
        ssize_t tmp_sent = sendto(socket_fd, offset, buffer_sz, 0, info->ai_addr, info->ai_addrlen);

        if (-1 == tmp_sent)
        {
            // Error of sendto
            perror("UDP sendto");
            amount_sent = -1;
            break;
        }
        else
        {
            // Partial write, update amount_sent and redo while loop
            amount_sent += tmp_sent;
        }
    }
    
    return amount_sent;
}
// END OF SOURCE