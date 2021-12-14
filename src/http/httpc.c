#include <stdio.h>
#include <socket_factory.h>

int main(void)
{
    socket_factory_t * factory = factory_init("127.0.0.1", "44567");
    server_connect(factory, TCP);
    tcp_send_msg(factory, "GET /index.html HTTP/1.1\r\n");
    factory_destroy(factory);
}
