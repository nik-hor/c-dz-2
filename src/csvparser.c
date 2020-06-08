#include "csvparser.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// инициализация
CsvParser *init_parser(const char *filePath, const char *delimiter, errors_t *status) {
    if (filePath == NULL) {
        *status = NULL_FILE_PATH;
        return NULL;
    }

    CsvParser *csvParser = malloc(sizeof(CsvParser));
    if (csvParser == NULL) {
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    strcpy(csvParser->file_path, filePath);

    if (delimiter == NULL) {
        csvParser->delim = ',';
    } else if (is_delim_valid(delimiter)) {
        csvParser->delim = *delimiter;
    } else {
        csvParser->delim = '\0';
    }
    csvParser->file = NULL;
    csvParser->string_index = 0;
    csvParser->err = OK;

    return csvParser;
}

// деинициализация
void free_parser(CsvParser *csvParser) {
    if (csvParser == NULL) {
        return;
    }
    if (csvParser->file != NULL) {
        fclose(csvParser->file);
    }
    free(csvParser);
}

// освобождает память из под строки
void destroy_row(CsvRow *csvRow) {
    for (int i = 0; i < csvRow->num_fields; ++i) {
        free(csvRow->fields[i]);
    }
    free(csvRow->fields);
    free(csvRow);
}

// возвращает количество полей
int get_num_fields(const CsvRow *csvRow) {
    return csvRow->num_fields;
}

// возвращает строки
const char **get_fields(const CsvRow *csvRow) {
    return (const char **)csvRow->fields;
}

CsvRow *get_row(CsvParser *csvParser) {
    int numRowRealloc = 0;
    int max_fields = 64;
    int max_field_size = 64;

    if (csvParser->delim == '\0') {
        csvParser->err = UNSUPPORTED_DELIM;
        return NULL;
    }
    if (csvParser->file == NULL) {
        csvParser->file = fopen(csvParser->file_path, "r");
        if (csvParser->file == NULL) {
            csvParser->err = FILE_OPEN_ERR;
            return NULL;
        }
    }
    CsvRow *csvRow = malloc(sizeof(CsvRow));
    if (!csvRow) {
        return NULL;
    }
    csvRow->fields = malloc(max_fields * sizeof(char *));
    if (!csvRow->fields) {
        free(csvRow);
        return NULL;
    }

    csvRow->num_fields = 0;
    int fieldIter = 0;

    char *cur_field = (char *)malloc(max_field_size);  //текущее поле
    if (cur_field == NULL) {
        destroy_row(csvRow);
        return NULL;
    }
    int inside_complex_field = 0;  // индикатор сложного поля (;"*";)
    int cur_feld_pos = 0; // Текущая позиция в поле
    int quotes_cnt = 0; // счётчик кавычек
    int last_char_quote = 0; // последний символ кавычка
    int end_of_file = 0; // достигнут ли конеч файла
    while (1) {
        char cur_char = fgetc(csvParser->file); // считали символ из файла
        csvParser->string_index++; // сместились на следующий
        if (feof(csvParser->file)) { // если считали конеч файла - завершаем работу
            if (cur_feld_pos == 0 && fieldIter == 0) {
                free(cur_field);
                destroy_row(csvRow);
                return NULL;
            }
            cur_char = '\n';
            end_of_file = 1;
        }
        if (cur_char == '\r') { // обработка \r
            continue;
        }
        if (cur_feld_pos == 0 && !last_char_quote) { // если мы в начале поля и последний прочитанный символ не кавычка
            if (cur_char == '\"') { // если текущий символ кавычка
                inside_complex_field = 1; // сигнализируем сложное поле
                last_char_quote = 1;// отмечаем кавычку
                continue; // продолжаем
            }
        } else if (cur_char == '\"') {  // наткнулись на кавчку
            quotes_cnt++; // посчитали
            inside_complex_field = (quotes_cnt % 2 == 0); // если чётное количество - вышли из сложного поля
            if (inside_complex_field) {
                cur_feld_pos--;
            }
        } else {
            quotes_cnt = 0;
        }
        // если достигнут конец файла или разделитель или перезод на новую строку (и при этом мы не в сложном поле)
        // значит поле мы считали и можно его записать
        if (end_of_file || ((cur_char == csvParser->delim || cur_char == '\n') && !inside_complex_field)) {
            cur_field[last_char_quote ? cur_feld_pos - 1 : cur_feld_pos] = '\0'; // не записываем кавычку
            csvRow->fields[fieldIter] = (char *)malloc(cur_feld_pos + 1);
            strcpy(csvRow->fields[fieldIter], cur_field); // копируем поле
            free(cur_field);
            csvRow->num_fields++;
            if (cur_char == '\n') { // если в конче переход на новую строку, значит мы считали всю строку
                return csvRow;
            }
            if (csvRow->num_fields != 0 && csvRow->num_fields % max_fields == 0) { // если места не хватает и нужно больше
                csvRow->fields = realloc(csvRow->fields, ((numRowRealloc + 2) * max_fields) * sizeof(char *));
                if (csvRow->fields == NULL) {
                    destroy_row(csvRow);
                    if (cur_field) {
                        free(cur_field);
                    }
                    return NULL;
                }
                numRowRealloc++;
            }
            // если не новая строка, значит мы считали поле
            // и нужно выделить память под следующее
            max_field_size = 64;
            cur_field = malloc(max_field_size);
            if (cur_field == NULL) {
                destroy_row(csvRow);
                free(cur_field);
                return NULL;
            }
            cur_feld_pos = 0; // сбрасываем индекс на начало поля
            fieldIter++; // увеличиваем количество полей
            inside_complex_field = 0;
        } else { // если не конеч файла, поля, или не новая строка
            cur_field[cur_feld_pos] = cur_char; // записываем символ поля
            cur_feld_pos++;
            if (cur_feld_pos == max_field_size - 1) { // если не хватило места
                max_field_size *= 2;
                cur_field = realloc(cur_field, max_field_size);
                if (cur_field == NULL) {
                    destroy_row(csvRow);
                    free(cur_field);
                    return NULL;
                }
            }
        }
        last_char_quote = (cur_char == '\"') ? 1 : 0; // устанавливаем флаг, что последним символом (перед новым) была кавычка
    }
}

// проверка на валидность разделителя
int is_delim_valid(const char *delim) {
    if (*delim == '\n' || *delim == '\r' || *delim == '\0' || *delim == '\"') {
        return 0;
    }
    return 1;
}
