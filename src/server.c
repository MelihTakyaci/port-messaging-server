#include "server.h"
#include "config.h"
#include "logging.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "ssl_utils.h"
// Global server değişkenleri
volatile int server_running = 1;
int server_fd;
SSL_CTX *global_ssl_ctx = NULL;

// Global client list ve mutex (client.c ile paylaşılacak)
ClientNode *client_list_head = NULL;
pthread_mutex_t client_list_mutex = PTHREAD_MUTEX_INITIALIZER;

void handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\nGracefully shutting down server...\n");
        log_message("Server is shutting down gracefully");
        server_running = 0;
        if (server_fd > 0) {
            close(server_fd);
        }
    }
}

int main(int argc, char *argv[]) {
    ServerConfig config;
    struct sockaddr_in server_addr;
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int opt_val = 1;
    pthread_t thread_id;
    
    // Log dosyasını başlat ve log kaydı tut
    init_log("server.log");
    log_message("Server is starting up");
    
    // Initialize SSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    global_ssl_ctx = create_ssl_context(1); // 1 = server
    configure_context(global_ssl_ctx, "cert.pem", "key.pem");


    // Komut satırı argümanlarını işle
    parse_arguments(argc, argv, &config);
    
    // Sinyal yakalayıcıları
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    // Sunucu soketi oluştur
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        switch(errno) {
            case EACCES:
                fprintf(stderr, "Permission error! Try running as root.\n");
                break;
            case EAFNOSUPPORT:
                fprintf(stderr, "IPv6 not supported.\n");
                break;
            default:
                perror("Socket creation failed");
        }
        exit(EXIT_FAILURE);
    }
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config.port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));
    
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        switch(errno) {
            case EADDRINUSE:
                fprintf(stderr, "Port %d is already in use!\n", config.port);
                break;
            case EACCES:
                fprintf(stderr, "Root privileges required for ports below 1024!\n");
                break;
            case EINVAL:
                fprintf(stderr, "Socket is already bound or invalid address.\n");
                break;
            case ENOTSOCK:
                fprintf(stderr, "File descriptor is not a valid socket.\n");
                break;
            case EADDRNOTAVAIL:
                fprintf(stderr, "The specified IP address is unavailable.\n");
                break;
            default:
                perror("Bind failed");
        }
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    int system_max_backlog = 128;
    int actual_backlog = (config.backlog_size > system_max_backlog) ? system_max_backlog : config.backlog_size;
    
    if (listen(server_fd, actual_backlog) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d with backlog size %d\n", config.port, actual_backlog);
    log_message("Server listening on port %d with backlog size %d", config.port, actual_backlog);
    printf("Press Ctrl+C to stop the server\n");
    
    while (server_running) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (!server_running) break;
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            perror("accept failed");
            log_message("accept failed: %s", strerror(errno));
            continue;
        }
    
        ClientInfo *client = (ClientInfo *)malloc(sizeof(ClientInfo));
        if (!client) {
            perror("Failed to allocate memory for client");
            close(client_fd);
            continue;
        }
    
        client->client_fd = client_fd;
        client->client_addr = client_addr;
    
        add_client(client);
    
        ThreadArg *thread_arg = (ThreadArg *)malloc(sizeof(ThreadArg));
        if (!thread_arg) {
            perror("Failed to allocate memory for thread argument");
            remove_client(client->client_fd);
            free(client);
            close(client_fd);
            continue;
        }
    
        thread_arg->client = client;
        thread_arg->ssl_ctx = global_ssl_ctx;
    
        if (pthread_create(&thread_id, NULL, handle_client, (void *)thread_arg) != 0) {
            perror("pthread_create failed");
            remove_client(client->client_fd);
            free(client);
            free(thread_arg);
            close(client_fd);
            continue;
        }
    
        pthread_detach(thread_id);
    }
    
    
    printf("Server shut down successfully\n");
    log_message("Server shut down successfully");
    if (server_fd > 0) {
        close(server_fd);
    }
    
    if (global_ssl_ctx) {
        SSL_CTX_free(global_ssl_ctx);
        global_ssl_ctx = NULL;
    }    
    close_log();
    return 0;
}
