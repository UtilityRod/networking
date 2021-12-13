#include <stdio.h>
#include <socket_factory.h>

int main(void)
{
    socket_factory_t * factory = factory_init("127.0.0.1", "36482");
    tcp_server_setup(factory);
    tcp_accept(factory);
    factory_destroy(factory);
}
