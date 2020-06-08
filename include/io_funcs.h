#ifndef IO_FUNCS_H
#define IO_FUNCS_H

#include <stdbool.h>
#include <stdio.h>

#include "errors.h"

errors_t read_string(char* dst, size_t size, FILE* stream);
errors_t read_size_t(size_t* dst);
errors_t read_int(int* dst);

// Разедлы с меню
void top_level_menu();
void book_menu(bool full_access);
void student_menu(bool full_access);

#endif
