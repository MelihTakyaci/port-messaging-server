#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>
#include <time.h>
#include <openssl/sha.h>

#define BUFFER_SIZE 2048

typedef struct {
    int client_fd;
    struct sockaddr_in client_addr;
    int request_count;   // Mevcut 10 saniyelik pencere içinde istek sayısı
    time_t window_start; // 10 saniyelik pencerenin başlangıcı
} ClientInfo;

typedef struct ClientNode {
    ClientInfo *client;
    struct ClientNode *next;
} ClientNode;

void add_client(ClientInfo *client);
void remove_client(int client_fd);
void broadcast_message(ClientInfo *sender, const char *message, int message_length);
void *handle_client(void *arg);

#endif // CLIENT_H
