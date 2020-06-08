#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <stdio.h>
#include "errors.h"

typedef struct CsvRow {
    char **fields; // поля
    int num_fields; // количество полей
} CsvRow;

typedef struct CsvParser {
    char file_path[INIT_SIZE]; // путь к файлу
    char delim; // разделитель
    errors_t err; // текущая ошибка
    FILE *file; // дескриптор
	int string_index; // положение в строке
} CsvParser;

CsvParser *init_parser(const char *filePath, const char *delimiter, errors_t* status);
void free_parser(CsvParser *csvParser);
void destroy_row(CsvRow *csvRow);
CsvRow *get_row(CsvParser *csvParser);
int get_num_fields(const CsvRow *csvRow);
const char **get_fields(const CsvRow *csvRow);

int is_delim_valid(const char *delim);


#endif
