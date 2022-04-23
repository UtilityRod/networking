#ifndef _SSL_OPERATIONS_H_
#define _SSL_OPERATIONS_H_

#include <stddef.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define SAFECTXFREE(x) if(x != NULL) {SSL_CTX_free(x);}
#define SAFESSLFREE(x) if(x != NULL) {SSL_free(x);}

typedef enum {SERVER, CLIENT} ssl_type_t;

void openssl_init(void);
SSL_CTX * openssl_get_ctx(ssl_type_t ssl_type);
int openssl_load_certificate(SSL_CTX * ctx, char * cert_path, char * key_path);
SSL * openssl_get_ssl(SSL_CTX * ctx, int socket_fd);
void openssl_ssl_cleanup(SSL * ssl);
void openssl_ctx_cleanup(SSL_CTX * ctx);
int openssl_tcp_read_all(SSL * ssl, void * buffer, size_t buffer_sz);
int openssl_tcp_send_all(SSL * ssl, void * buffer, size_t buffer_sz);
void openssl_print_error(SSL * ssl, int return_code);

#endif