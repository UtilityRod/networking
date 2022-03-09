#include <test_main.h>

int main(void)
{
    socket_factory_t * factory = factory_create("44567");
    tcp_server_setup(factory);
    tcp_accept_fork(factory, "fork_test");
    factory_destroy(factory);
}