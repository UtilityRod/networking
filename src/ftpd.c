#include "socket_factory.h"
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

void establish_handler(int sig, void (*func)(int));
void signal_handler(int sig);

// Flag used to exit when CTL + C (SIGINT) is caught
volatile sig_atomic_t accept_flag = 0;

int main(void)
{
    // Set signal handler to catch SIGCHLD and SIGINT
    establish_handler(SIGCHLD, signal_handler);
    establish_handler(SIGINT, signal_handler);
    // Setup socket to listen at given IP and Port
    socket_factory_t * factory = factory_init("127.0.0.1", "44567");
    // Only if socket is greater than 0 
    // will you try to accept connections
    if (tcp_server_setup(factory) > 0)
    {
        while (1) 
        {
            tcp_fork_listen(factory);
            if (accept_flag != 0) 
            {
                break;
            }
        }
        // Once returned shutdown and close the socket
        factory_destroy(factory);
    }
}

void establish_handler(int sig, void (*func)(int))
{
    // Set struct sigaction to contain given function
    struct sigaction sa = { .sa_handler = func};
    // Call sigaction to set signal handler to signal
    if (sigaction(sig, &sa, NULL) == -1)
    {
        perror("Signal action failed.");
        exit(-1);
    }
}

void signal_handler(int sig)
{
    switch (sig)
    {
        case SIGINT: ;
            // If CTL+C caught, set accept_flag for exit
            accept_flag = 1;
            break;
        case SIGCHLD: ;
            // Child process exited, call wait to get child process
            // exit value for OS to remove zombie process.
            int num = -1;
            wait(&num);
            break;
        default:
            fprintf(stderr, "Unknown signal caught: %d\n", sig);
    }
    
}
// END OF SOURCE
