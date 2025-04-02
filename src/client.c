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
            if (send(curr->client->client_fd, message, message_length, 0) < 0) {
                perror("Broadcast send failed");
            }
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&client_list_mutex);
}

void *handle_client(void *arg) {
    ClientInfo *client = (ClientInfo *)arg;
    char buffer[BUFFER_SIZE];
    int bytes_received;
    char client_ip[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, &(client->client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Client connected from %s:%d\n", client_ip, ntohs(client->client_addr.sin_port));
    log_message("Client connected from %s:%d", client_ip, ntohs(client->client_addr.sin_port));
    
    // Rate limiting için başlangıç ayarları
    client->window_start = time(NULL);
    client->request_count = 0;
    
    while ((bytes_received = recv(client->client_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        
        // Rate limiting: 10 saniyede maksimum 15 istek
        time_t now = time(NULL);
        if (now - client->window_start >= 10) {
            client->window_start = now;
            client->request_count = 0;
        }
        if (client->request_count >= 15) {
            char *limit_msg = "Rate limit exceeded. Please wait 10 seconds before sending more messages.\n";
            send(client->client_fd, limit_msg, strlen(limit_msg), 0);
            continue;
        }
        client->request_count++;
        
        // Gelen verinin SHA256 hash'ini hesaplama (sadece loglama için)
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256((unsigned char*)buffer, bytes_received, hash);
        char hash_str[SHA256_DIGEST_LENGTH * 2 + 1];
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            sprintf(&hash_str[i * 2], "%02x", hash[i]);
        }
        hash_str[SHA256_DIGEST_LENGTH * 2] = '\0';
        
        log_message("Received from %s:%d: %s", client_ip, ntohs(client->client_addr.sin_port), buffer);
        log_message("Hash of received message: %s", hash_str);
        
        // Yayınlanacak mesajın hazırlanması
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
        perror("recv failed");
        log_message("recv failed for client %s:%d", client_ip, ntohs(client->client_addr.sin_port));
    }
    
    remove_client(client->client_fd);
    close(client->client_fd);
    free(client);
    pthread_exit(NULL);
}
