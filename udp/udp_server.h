#ifndef _UDP_SERVER_H_
#define _UDP_SERVER_H_

#include <stddef.h>
#include <unistd.h>

typedef struct udp_server udp_server_t;

udp_server_t * udp_server_setup(char * port);
void udp_server_teardown(udp_server_t * server);
ssize_t udp_server_recv_all(udp_server_t * server, char * buffer, size_t max);

#endif