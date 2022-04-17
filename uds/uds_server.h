#ifndef _UDS_SERVER_H_
#define _UDS_SERVER_H_

typedef struct uds_server uds_server_t;

uds_server_t * uds_server_setup(char * socket_file);
void uds_server_teardown(uds_server_t * server);
int uds_server_accept(uds_server_t * server);

#endif