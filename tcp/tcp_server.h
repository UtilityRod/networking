#ifndef _SOCKET_FACTORY_H
#define _SOCKET_FACTORY_H

typedef struct tcp_server_ tcp_server_t;

tcp_server_t * tcp_server_setup(const char * port);
void tcp_server_teardown(tcp_server_t * server);
int tcp_server_accept(tcp_server_t * server);
void tcp_server_accept_fork(tcp_server_t * server, char * p_exe);

#endif
