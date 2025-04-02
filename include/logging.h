#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <stdarg.h>

void init_log(const char *filename);
void close_log();
void log_message(const char *format, ...);

#endif // LOGGING_H
