#include "socket_factory.h"
#include <stdio.h>

int main(void)
{
    char msg[] = "Hello from client\n";
    socket_factory_t * factory = factory_init("0.0.0.0", "44567");
    udp_send_msg(factory, msg);
    factory_destroy(factory);
}
