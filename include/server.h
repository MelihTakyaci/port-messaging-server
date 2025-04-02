#ifndef SERVER_H
#define SERVER_H

#include "config.h"

extern volatile int server_running;
extern int server_fd;

void handle_signal(int sig);

#endif // SERVER_H
