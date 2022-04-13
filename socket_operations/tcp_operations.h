#ifndef _TCP_OPERATIONS_H_
#define _TCP_OPERATIONS_H_

#include <stddef.h>

int tcp_read_all(int socket_fd, char * buffer, size_t buffer_sz);
int tcp_send_all(int socket_fd, char * buffer, size_t buffer_sz);

#endif