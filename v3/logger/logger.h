// logger.h

#ifndef LOGGER_H
#define LOGGER_H

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>

typedef enum { LOG_INFO, LOG_WARNING, LOG_ERROR } LogLevel;

#define DEFAULT_LOG_FILE "server_logs.txt"

void init_logger(const char *filename);
void close_logger();
void log_message(LogLevel level, const char *format, ...);

#endif /* LOGGER_H */
