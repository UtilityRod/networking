#include <stdio.h>
#include <socket_factory.h>

int main(void)
{
    socket_factory_t * factory = factory_init("127.0.0.1", "36482");
    server_connect(factory, TCP);
    tcp_send_msg(factory, "Hello from client!");
    factory_destroy(factory);
}
