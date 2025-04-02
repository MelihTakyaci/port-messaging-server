#include "logging.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

FILE *log_file = NULL;

void init_log(const char *filename) {
    log_file = fopen(filename, "a"); // Append modunda açılıyor.
    if (!log_file) {
        perror("Log dosyası açılamadı");
        exit(EXIT_FAILURE);
    }
}

void close_log() {
    if (log_file) {
        fclose(log_file);
    }
}

void log_message(const char *format, ...) {
    if (!log_file)
        return;
    
    time_t now = time(NULL);
    char time_buf[64];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    fprintf(log_file, "[%s] ", time_buf);
    
    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
    
    fprintf(log_file, "\n");
    fflush(log_file);
}
