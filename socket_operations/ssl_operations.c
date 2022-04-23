#include <ssl_operations.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>

void openssl_init(void)
{
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
}

SSL_CTX * openssl_get_ctx(ssl_type_t ssl_type)
{
    SSL_CTX * ctx = NULL;

    switch (ssl_type)
    {
        case SERVER:
            ctx = SSL_CTX_new(TLS_server_method());
            break;
        case CLIENT:
            ctx = SSL_CTX_new(TLS_client_method());
            break;
        default:
            break;
    }

    return ctx;
}

int openssl_load_certificate(SSL_CTX * ctx, char * cert_path, char * key_path)
{
    int certificate_check = SSL_CTX_use_certificate_file(ctx, cert_path, SSL_FILETYPE_PEM);
    int key_check = SSL_CTX_use_PrivateKey_file(ctx, key_path, SSL_FILETYPE_PEM);
    return ((1 == certificate_check) && (1 == key_check));
}
    
SSL * openssl_get_ssl(SSL_CTX * ctx, int socket_fd)
{
    SSL * ssl = NULL;

    if (ctx != NULL)
    {
        ssl = SSL_new(ctx);
    }

    if (ssl != NULL)
    {
        int set_fd_check = SSL_set_fd(ssl, socket_fd);
        if (0 == set_fd_check)
        {
            SAFESSLFREE(ssl);
            ssl = NULL;
        }
    }

    return ssl;
}

int openssl_tcp_read_all(SSL * ssl, void * buffer, size_t buffer_sz)
{
    if ((NULL == buffer) || (0 == buffer_sz) || (NULL == ssl))
    {
        return -1;
    }

    uint8_t * internal_buffer = (uint8_t *)buffer;
    int amount_read = 0;
    int rv = 0;
    while (amount_read < buffer_sz)
    {
        char * read_buffer = internal_buffer + amount_read;
        int read_size = buffer_sz - amount_read;
        int tmp_read = SSL_read(ssl, read_buffer, read_size);

        if (-1 == tmp_read)
        {
            rv = -1;
            break;
        }
        else
        {
            amount_read += tmp_read;
        }
    }
    return (rv != -1) ? amount_read : rv;
}

int openssl_tcp_send_all(SSL * ssl, void * buffer, size_t buffer_sz)
{
    if ((NULL == buffer) || (0 == buffer_sz) || (NULL == ssl))
    {
        return -1;
    }


    uint8_t * internal_buffer = (uint8_t *)buffer;
    size_t amount_sent = 0;
    int rv = 0;
    while (amount_sent < buffer_sz)
    {
        char * write_data = internal_buffer + amount_sent;
        size_t write_sz = buffer_sz - amount_sent;
        int tmp_sent = SSL_write(ssl, write_data, write_sz);
        if (tmp_sent == -1)
        {
            ERR_print_errors_fp(stderr);
            rv = -1;
            break;
        }
        else
        {
            amount_sent += tmp_sent;
        }
    }

    return (rv != -1) ? amount_sent : rv;
}

void openssl_print_error(SSL * ssl, int return_code)
{
    int error = SSL_get_error(ssl, return_code);
    switch (error)
    {
        case SSL_ERROR_NONE:
            printf("SSL_ERROR_NONE\n");
            break;
        case SSL_ERROR_ZERO_RETURN:
            printf("SSL_ERROR_ZERO_RETURN\n");
            break;
        case SSL_ERROR_WANT_READ:
            printf("SSL_ERROR_WANT_READ\n");
            break;
        case SSL_ERROR_WANT_WRITE:
            printf("SSL_ERROR_WANT_WRITE\n");
            break;
        case SSL_ERROR_WANT_CONNECT:
            printf("SSL_ERROR_WANT_CONNECT\n");
            break;
        case SSL_ERROR_WANT_ACCEPT:
            printf("SSL_ERROR_WANT_ACCEPT\n");
            break;
        case SSL_ERROR_WANT_X509_LOOKUP:
            printf("SSL_ERROR_WANT_X509_LOOKUP\n");
            break;
        case SSL_ERROR_WANT_ASYNC:
            printf("SSL_ERROR_WANT_ASYNC\n");
            break;
        case SSL_ERROR_WANT_ASYNC_JOB:
            printf("SSL_ERROR_WANT_ASYNC_JOB\n");
            break;
        case SSL_ERROR_WANT_CLIENT_HELLO_CB:
            printf("SSL_ERROR_WANT_CLIENT_HELLO_CB\n");
            break;
        case SSL_ERROR_SYSCALL:
            printf("SSL_ERROR_SYSCALL\n");
            break;
        case SSL_ERROR_SSL:
            printf("SSL_ERROR_SSL\n");
            break;
        default:
            printf("Unknown error.\n");
            break;
    }
}
// END OF SOURCE