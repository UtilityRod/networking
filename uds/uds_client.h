#ifndef _UDS_CLIENT_H_
#define _UDS_CLIENT_H_

typedef struct uds_client uds_client_t;

uds_client_t * uds_client_setup(char * socket_file);
void uds_client_teardown(uds_client_t * client);
int uds_client_get_socket(uds_client_t * client);

#endif