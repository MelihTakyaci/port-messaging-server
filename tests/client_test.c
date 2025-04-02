// tests/client_test.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 2048

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Soket oluştur
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Sunucu adresi ayarla
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Sunucuya bağlan
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        close(sock);
        return 1;
    }

    printf("Connected to server.\n");

    // Mesaj gönder
    const char *message = "Hello from test client!";
    send(sock, message, strlen(message), 0);

    // Yanıt al
    int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        perror("Receive failed");
    } else {
        buffer[bytes_received] = '\0';
        printf("Received from server: %s\n", buffer);
    }

    // Bağlantıyı kapat
    close(sock);
    return 0;
}
