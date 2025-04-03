#include "client.h"
#include "logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <openssl/sha.h>
#include <sys/socket.h>
#include "ssl_utils.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/sha.h>


// Global client list ve mutex tanımları (server.c’de de kullanılacak)
extern ClientNode *client_list_head;
extern pthread_mutex_t client_list_mutex;

void add_client(ClientInfo *client) {
    ClientNode *new_node = malloc(sizeof(ClientNode));
    if (!new_node) {
        perror("add_client: malloc failed");
        exit(EXIT_FAILURE);
    }
    new_node->client = client;
    pthread_mutex_lock(&client_list_mutex);
    new_node->next = client_list_head;
    client_list_head = new_node;
    pthread_mutex_unlock(&client_list_mutex);
}

void remove_client(int client_fd) {
    pthread_mutex_lock(&client_list_mutex);
    ClientNode **curr = &client_list_head;
    while (*curr) {
        if ((*curr)->client->client_fd == client_fd) {
            ClientNode *to_remove = *curr;
            *curr = to_remove->next;
            free(to_remove);
            break;
        }
        curr = &((*curr)->next);
    }
    pthread_mutex_unlock(&client_list_mutex);
}

void broadcast_message(ClientInfo *sender, const char *message, int message_length) {
    pthread_mutex_lock(&client_list_mutex);
    ClientNode *curr = client_list_head;
    while (curr) {
        if (curr->client->client_fd != sender->client_fd) {
            if (SSL_write(curr->client->ssl, message, message_length) <= 0) {
                perror("Broadcast send failed");
            }
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&client_list_mutex);
}

void *handle_client(void *arg) {
    ThreadArg *thread_arg = (ThreadArg *)arg;
    ClientInfo *client = thread_arg->client;
    SSL_CTX *ctx = thread_arg->ssl_ctx;
    free(thread_arg);

    char buffer[BUFFER_SIZE];
    int bytes_received;
    char client_ip[INET_ADDRSTRLEN];

    client->ssl = SSL_new(ctx);
    if (!client->ssl || SSL_set_fd(client->ssl, client->client_fd) != 1) {
        ERR_print_errors_fp(stderr);
        close(client->client_fd);
        free(client);
        pthread_exit(NULL);
    }

    if (SSL_accept(client->ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(client->ssl);
        close(client->client_fd);
        free(client);
        pthread_exit(NULL);
    }

    // Log client IP
    inet_ntop(AF_INET, &(client->client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Client connected from %s:%d\n", client_ip, ntohs(client->client_addr.sin_port));
    log_message("Client connected from %s:%d", client_ip, ntohs(client->client_addr.sin_port));

    // Rate limiting setup
    client->window_start = time(NULL);
    client->request_count = 0;

    while ((bytes_received = SSL_read(client->ssl, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_received] = '\0';

        // Rate limiting: 10 seconds window, max 15 requests
        time_t now = time(NULL);
        if (now - client->window_start >= 10) {
            client->window_start = now;
            client->request_count = 0;
        }
        if (client->request_count >= 15) {
            const char *limit_msg = "Rate limit exceeded. Please wait 10 seconds before sending more messages.\n";
            SSL_write(client->ssl, limit_msg, strlen(limit_msg));
            continue;
        }
        client->request_count++;

        // Calculate SHA256 of received message
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256((unsigned char*)buffer, bytes_received, hash);
        char hash_str[SHA256_DIGEST_LENGTH * 2 + 1];
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            sprintf(&hash_str[i * 2], "%02x", hash[i]);
        }
        hash_str[SHA256_DIGEST_LENGTH * 2] = '\0';

        log_message("Received from %s:%d: %s", client_ip, ntohs(client->client_addr.sin_port), buffer);
        log_message("Hash of received message: %s", hash_str);

        // Prepare broadcast message
        char broadcast_buffer[BUFFER_SIZE * 2];
        snprintf(broadcast_buffer, sizeof(broadcast_buffer),
                 "From %s:%d -> Message: %s\n",
                 client_ip, ntohs(client->client_addr.sin_port), buffer);

        broadcast_message(client, broadcast_buffer, strlen(broadcast_buffer));
    }

    if (bytes_received == 0) {
        printf("Client %s:%d disconnected\n", client_ip, ntohs(client->client_addr.sin_port));
        log_message("Client %s:%d disconnected", client_ip, ntohs(client->client_addr.sin_port));
    } else {
        perror("SSL_read failed");
        log_message("SSL_read failed for client %s:%d", client_ip, ntohs(client->client_addr.sin_port));
    }

    remove_client(client->client_fd);

    // Cleanup SSL
    SSL_shutdown(client->ssl);
    SSL_free(client->ssl);

    close(client->client_fd);
    free(client);
    pthread_exit(NULL);
}
