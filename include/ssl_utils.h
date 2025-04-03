#ifndef SSL_UTILS_H
#define SSL_UTILS_H

#include <openssl/ssl.h>

SSL_CTX *create_ssl_context(int is_server);
void configure_context(SSL_CTX *ctx, const char *cert_file, const char *key_file);
SSL *accept_tls_connection(SSL_CTX *ctx, int client_fd);   // for server
SSL *connect_tls_to_server(SSL_CTX *ctx, int socket_fd);   // for client
void cleanup_ssl(SSL *ssl);

#endif
