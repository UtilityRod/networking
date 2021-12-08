#include "socket_factory.h"
#include <stdio.h>

int main(void)
{
    char msg[] = "Hello from client\n";
    socket_factory_t * factory = factory_init("127.0.0.1", "44567");
    server_connect(factory, UDP);
    udp_send_msg(factory, msg);
    factory_destroy(factory);
}
