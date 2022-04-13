#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include <stddef.h>

typedef struct tcp_client tcp_client_t;

tcp_client_t * tcp_client_setup(const char * address, const char * port);
void tcp_client_teardown(tcp_client_t * client);
int tcp_client_connect(tcp_client_t * client);
void tcp_client_disconnect(tcp_client_t * client);

#endif