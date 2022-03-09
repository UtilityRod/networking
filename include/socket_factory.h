#ifndef _SOCKET_FACTORY_H
#define _SOCKET_FACTORY_H

typedef struct socket_factory_ socket_factory_t;
socket_factory_t * factory_init(const char * p_ip, const char * p_port);

#endif
