#ifndef _SOCKET_FACTORY_H
#define _SOCKET_FACTORY_H

typedef struct socket_factory_ socket_factory_t;
typedef enum {TCP, UDP} socket_type;

socket_factory_t * factory_init(const char * ip_addr, const char * port);
void factory_destroy(socket_factory_t * factory);
int factory_get_socket(socket_factory_t * factory);
int server_connect(socket_factory_t * factory, socket_type type);
char * udp_recv_msg(socket_factory_t * factory);
int udp_send_msg(socket_factory_t * factory, char * msg);
int tcp_server_setup(socket_factory_t * factory);
void tcp_fork_listen(socket_factory_t * factory);
int udp_server_setup(socket_factory_t * factory);


#endif
