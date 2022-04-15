#ifndef _UDP_CLIENT_H_
#define _UDP_CLIENT_H_

#include <unistd.h>

typedef struct udp_client udp_client_t;

udp_client_t * udp_client_setup(char * server, char * port);
void udp_client_teardown(udp_client_t * client);
ssize_t udp_client_send(udp_client_t * client, char * buffer, size_t buffer_sz);
ssize_t udp_client_read(udp_client_t * client, char * buffer, size_t buffer_sz);

#endif