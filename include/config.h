#ifndef CONFIG_H
#define CONFIG_H

#define DEFAULT_PORT 8080
#define DEFAULT_BACKLOG_SIZE 128
#define MAX_CONNECTIONS 100

typedef struct {
    int port;
    int backlog_size;
    int max_connections;
} ServerConfig;

void show_usage(const char* program_name);
void parse_arguments(int argc, char *argv[], ServerConfig *config);

#endif // CONFIG_H
