#include <test_main.h>

int main(void)
{
    socket_factory_t * factory = factory_create("44567");
    tcp_server_setup(factory);
    factory_destroy(factory);
}