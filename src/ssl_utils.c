#include "ssl_utils.h"
#include <openssl/err.h>
#include <stdlib.h>
#include <stdio.h>

SSL_CTX *create_ssl_context(int is_server) {
    const SSL_METHOD *method = is_server ? TLS_server_method() : TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx, const char *cert_file, const char *key_file) {
    if (SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

SSL *accept_tls_connection(SSL_CTX *ctx, int client_fd) {
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_fd);

    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    return ssl;
}

SSL *connect_tls_to_server(SSL_CTX *ctx, int socket_fd) {
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, socket_fd);

    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    return ssl;
}

void cleanup_ssl(SSL *ssl) {
    SSL_shutdown(ssl);
    SSL_free(ssl);
}
