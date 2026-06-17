#include "logger.h"


static void get_timestamp(char *buffer, size_t max_len) {
    time_t raw_time;
    struct tm *time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(buffer, max_len, "%Y-%m-%d %H:%M:%S", time_info);
}

void log_info(const char *message) {
    FILE *log_file = fopen("app.log", "a");
    if (!log_file) return;

    char timestamp[20];
    get_timestamp(timestamp, sizeof(timestamp));

    fprintf(log_file, "[%s] [INFO] %s\n", timestamp, message);
    fclose(log_file);
}

void log_error(const char *message) {
    FILE *log_file = fopen("app.log", "a");
    if (!log_file) return;

    char timestamp[20];
    get_timestamp(timestamp, sizeof(timestamp));

    fprintf(log_file, "[%s] [ERROR] %s\n", timestamp, message);
    fclose(log_file);
}