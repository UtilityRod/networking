#include <socket_factory.h>
#include <stdio.h>
#include <stdlib.h>

struct socket_factory_ 
{
    const char * p_ip; // String IP address
    const char * p_port; // String port
    int socket_h; // The socket for communication
    struct addrinfo * p_results; // struct used for getaddrinfo()
};

socket_factory_t * factory_init(const char * p_ip, const char * p_port) 
{
    /*
     * Initialize the socket factory with default values and the values passed in
     * as arguments.
     */
    socket_factory_t * p_new = calloc(sizeof(*p_new), 1);
    
    if (p_new == NULL)
    {
        // malloc did not properly allocate memory
        perror("Error allocating memory for socket factory");
        return NULL;
    }
    else
    {
        // Set default values for struct data
        p_new->p_ip = p_ip;
        p_new->p_port = p_port;
        p_new->socket_h = -1;
        p_new->p_results = NULL;
    }
    
    
    return p_new;
}
