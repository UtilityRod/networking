#include "socket_factory.h"
#include <stdio.h>

int main(void)
{
    printf("Hello World!\n");
    socket_factory_t * factory = factory_init(NULL, "44567");
    udp_server_setup(factory);
    char * msg = udp_recv_msg(factory);
    printf("Message received: %s\n", msg);
    factory_destroy(factory);
}
