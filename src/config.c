#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

void show_usage(const char* program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("Options:\n");
    printf("  -p, --port PORT        Port to listen on (default: %d)\n", DEFAULT_PORT);
    printf("  -b, --backlog SIZE     Backlog queue size (default: %d)\n", DEFAULT_BACKLOG_SIZE);
    printf("  -h, --help             Show this help message\n");
}

void parse_arguments(int argc, char *argv[], ServerConfig *config) {
    config->port = DEFAULT_PORT;
    config->backlog_size = DEFAULT_BACKLOG_SIZE;
    config->max_connections = MAX_CONNECTIONS;
    
    static struct option long_options[] = {
        {"port",    required_argument, 0, 'p'},
        {"backlog", required_argument, 0, 'b'},
        {"help",    no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0, c;
    while ((c = getopt_long(argc, argv, "p:b:h", long_options, &option_index)) != -1) {
        switch (c) {
            case 'p':
                config->port = atoi(optarg);
                break;
            case 'b':
                config->backlog_size = atoi(optarg);
                break;
            case 'h':
                show_usage(argv[0]);
                exit(EXIT_SUCCESS);
            case '?':
                show_usage(argv[0]);
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }
}
