#ifndef _SOCKET_FACTORY_H
#define _SOCKET_FACTORY_H

typedef struct socket_factory_ socket_factory_t;
socket_factory_t * factory_create(const char * p_port);
void factory_destroy(socket_factory_t * factory);
int tcp_server_setup(socket_factory_t * p_factory);
int tcp_accept(socket_factory_t * p_factory);
void tcp_accept_fork(socket_factory_t * p_factory, char * p_exe);

#endif
