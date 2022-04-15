#ifndef _UDP_SERVER_H_
#define _UDP_SERVER_H_

#include <unistd.h>
#include <udp_operations.h>

typedef struct udp_server udp_server_t;

udp_server_t * udp_server_setup(char * port);
void udp_server_teardown(udp_server_t * server);
ssize_t udp_server_read_respond(udp_server_t * server, char * buffer, 
                                size_t buffer_sz, responder_t respond);

#endif