#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>

void log_set_fp(FILE *fp);

void log_action(const char *fmt, ...);

#endif
