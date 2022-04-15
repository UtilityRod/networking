#ifndef _UDP_OPERATIONS_H_
#define _UDP_OPERATIONS_H_

#include <unistd.h>
#include <netdb.h>

typedef int (*responder_t)(char * buffer, size_t buff_sz);

ssize_t udp_read_all(int socket_fd, char * buffer, 
                    size_t buffer_sz, responder_t respond);
ssize_t udp_send_all(int socket_fd, struct addrinfo * server_info, 
                    char * buffer, size_t size);

#endif