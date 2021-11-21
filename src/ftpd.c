#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

int socket_setup(const char * ip_addr, unsigned int port);
void handle_connections(unsigned int server_socket);
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
    int server_socket = socket_setup("127.0.0.1", 44567);
    // Only if socket is greater than 0 
    // will you try to accept connections
    if (server_socket > 0)
    {
        // Handle all connections on the socket
        handle_connections(server_socket);
        // Once returned shutdown and close the socket
        shutdown(server_socket, O_RDWR);
        close(server_socket);
    }

}

int socket_setup(const char * ip_addr, unsigned int port)
{
    // Create TCP socket using AF_INET family
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        // Socket not created properly
        perror("Socket creation.");
        return -1;
    }
    
    // Assign server address information to struct
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip_addr);
    
    // Bind socket to address
    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        // Socket bind failed
        perror("Socket Bind");
        close(socket_fd);
        return -1;
    }
    
    // Listen on socket
    if (listen(socket_fd, 0) < 0)
    {
        // Socket listen failed
        perror("Socket Listen");
        close(socket_fd);
        return -1;
    }
    
    // Return socket_fd for the connection handler
    return socket_fd;
}

void handle_connections(unsigned int server_socket)
{
    while (1)
    {
        // Accept connection on socket
        struct sockaddr_in client_addr;
        unsigned int client_sz = sizeof(client_addr);
        // socket_connection -> file descriptor for connected client
        int socket_connection = accept(server_socket, (struct sockaddr *)&client_addr, &client_sz);
        if (socket_connection < 0)
        {
            // Accept failed, check if accept flag was set
            if (accept_flag != 0)
            {
                // Flag set break out of while loop and return
                break;
            }
            else
            {
                // Flag not set unknown error, just continue
                continue;
            }
        }
        // PID of parent/child from fork()
        pid_t pid = fork();
        // If pid is 0 it is the child. Execute binary
        if (pid == 0)
        {
            // Binary executable path
            char executable[] = "./ftps";
            // Socket file descriptor string for cmd line arguments
            char * sock_str = malloc(sizeof(char) * 10);
            // Put socket connection FD into string
            sprintf(sock_str, "%d", socket_connection);
            // Create argv (args) and envp for execve command
            // Command line arguments:
            // 1. Executable name
            // 2. Socket connection file descriptor
            char * args[] = {executable, sock_str, '\0'};
            char * envp[] = {'\0'};
            // Call execve on child to perform executable task
            execve(executable, args, envp);
        }
        else
        {
            close(socket_connection);
        }
    }
    // There is nothing to return
    return;
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
