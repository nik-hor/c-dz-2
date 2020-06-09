#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <sys/time.h>
#include <time.h>

#include "logger.h"

// здесь хранится файловый дескриптор (куда писать лог)
static struct {
    FILE *fp;
} L;

void log_set_fp(FILE *fp) { // установить дексриптор
    L.fp = fp;
}

// сама функция лога
void log_action(const char *fmt, ...) {
    struct timeval tv;

    gettimeofday(&tv, NULL);
    int ms = tv.tv_usec / 1000;
    long long int sec = tv.tv_sec;
    struct tm *lt = localtime(&sec);
    
    if (L.fp) {
        va_list args;
        char buf[32];
        buf[strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", lt)] = 0; // инициализация буфера с датой и временем
        fprintf(L.fp, "%s.%03d: ", buf, ms); // записываем буфер
        va_start(args, fmt);
        vfprintf(L.fp, fmt, args); // системная функция, как fprintf только с использованием переменного числа аргументов
        va_end(args);
        fprintf(L.fp, "\n");
        fflush(L.fp);
    }
}
