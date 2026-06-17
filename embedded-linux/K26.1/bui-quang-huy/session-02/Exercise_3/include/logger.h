#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>

void log_info(const char *message);
void log_error(const char *message);

#endif // LOGGER_H