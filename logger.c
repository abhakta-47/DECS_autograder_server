// logger.c

#include "logger.h"
#include <stdlib.h>
#include <time.h>

#define MAX_LOG_MESSAGE_SIZE 1024

static FILE *log_file = NULL;
static pthread_mutex_t log_mutex;

void init_logger(const char *filename) {
    log_file = fopen(filename, "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(&log_mutex, NULL);
}

void close_logger() {
    fclose(log_file);
    pthread_mutex_destroy(&log_mutex);
}

void log_message(LogLevel level, const char *format, ...) {

    if (log_file == NULL) {
        init_logger(DEFAULT_LOG_FILE);
    }

    va_list args;
    va_start(args, format);

    char log_message[MAX_LOG_MESSAGE_SIZE];
    vsnprintf(log_message, MAX_LOG_MESSAGE_SIZE, format, args);

    va_end(args);

    time_t raw_time;
    struct tm *info;
    char time_buffer[20];

    time(&raw_time);
    info = localtime(&raw_time);
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", info);

    const char *severity_str;
    switch (level) {
    case LOG_INFO:
        severity_str = "INFO";
        break;
    case LOG_WARNING:
        severity_str = "WARNING";
        break;
    case LOG_ERROR:
        severity_str = "ERROR";
        break;
    default:
        severity_str = "UNKNOWN";
    }

    pthread_mutex_lock(&log_mutex);

    fprintf(log_file, "[%s] [%s] [Thread:%lu] %s\n", time_buffer, severity_str,
            pthread_self(), log_message);
    fflush(log_file);

    pthread_mutex_unlock(&log_mutex);
}
