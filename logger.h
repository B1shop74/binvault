#ifndef LOGGER_H
#define LOGGER_H

typedef enum{
    INFO,
    WARNING,
    ERROR,
    CRITICAL
}LogLevel;

int log_message(LogLevel level, const char* message, const char* argv1);

#endif