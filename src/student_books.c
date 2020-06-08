#include "student_books.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "csvparser.h"

void sbook_init_array_values(sbook_table_t *st) {
    for (size_t i = st->cur_size; i < st->max_size; ++i) {
        st->sbooks[i] = NULL;
    }
}

sbook_table_t *sbook_init_table(errors_t *status) {
    sbook_table_t *st = NULL;
    st = malloc(sizeof(sbook_table_t));
    if (st == NULL) {
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    st->sbooks = malloc(INIT_SIZE * sizeof(sbook_t *));
    if (st->sbooks == NULL) {
        free(st);
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    st->max_size = INIT_SIZE;
    st->cur_size = 0;

    sbook_init_array_values(st);

    *status = OK;
    return st;
}

errors_t sbook_read_row_into_sbook(const char **row, const int cnt, sbook_t *sbook) {
    errors_t status = OK;
    for (int i = 0; i < cnt && status == OK; ++i) {
        switch (i) {
            case SB_ISBN: {
                char *end;
                sbook->isbn = strtoul(row[i], &end, 10);
                if (*end != 0) {
                    status = CSV_READ_ERR;
                }
                break;
            }
            case SB_RECORD_BOOK: {
                if (strlen(row[i]) < MAX_RB_LEN) {
                    sbook->record_book = malloc((strlen(row[i]) + 1) * sizeof(char));
                    if (sbook->record_book) {
                        strcpy(sbook->record_book, row[i]);
                    } else {
                        status = MEM_ALLOC_ERR;
                    }
                } else {
                    status = CSV_READ_ERR;
                }
                break;
            }
            case SB_DATE: {
                status = sbook_split_date(&sbook->date, row[i]);
                break;
            }
        }
    }

    return status;
}

sbook_t *sbook_init(errors_t *status) {
    sbook_t *tmp = malloc(sizeof(sbook_t));
    if (tmp == NULL) {
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    tmp->record_book = NULL;

    return tmp;
}

errors_t sbook_extend_table(sbook_table_t *table, const size_t value) {
    if (value == 0) {
        return OK;
    }

    sbook_t **tmp = realloc(table->sbooks, (table->max_size + value) * sizeof(sbook_t *));
    if (tmp) {
        table->sbooks = tmp;
        sbook_init_array_values(table);
    } else {
        return MEM_ALLOC_ERR;
    }

    table->max_size += value;

    return OK;
}

sbook_table_t *sbook_read_csv(const char *file_path, const char *delim, errors_t *status) {
    sbook_table_t *sbook_table = sbook_init_table(status);
    if (sbook_table == NULL) {
        return NULL;
    }

    CsvParser *csvparser = init_parser(file_path, delim, status);

    if (*status != OK) {
        sbook_free_table(sbook_table);
        return NULL;
    }

    CsvRow *row;
    *status = OK;

    sbook_t *sbook = NULL;
    while ((row = get_row(csvparser)) && *status == OK) {
        const char **rowFields = get_fields(row);
        const int cnt = get_num_fields(row);
        sbook = sbook_init(status);
        if (sbook) {
            *status = sbook_read_row_into_sbook(rowFields, cnt, sbook);
            if (*status == OK) {
                destroy_row(row);
                if (sbook_table->cur_size == sbook_table->max_size) {
                    *status = sbook_extend_table(sbook_table, 50);
                }
                if (*status == OK) {
                    sbook_table->sbooks[sbook_table->cur_size] = sbook;
                    ++(sbook_table->cur_size);
                }
            }
        } else {
            *status = MEM_ALLOC_ERR;
        }
    }

    printf("Student books csv load: %s\n", str_err(csvparser->err));
    free_parser(csvparser);

    if (sbook && *status != OK) {
        sbook_free(sbook);
    }
    
    return sbook_table;
}

errors_t compress_sbook_table(sbook_table_t **table) {
    if ((*table)->max_size == (*table)->cur_size) {
        return OK;
    }

    sbook_t **tmp = realloc((*table)->sbooks, (*table)->cur_size * sizeof(sbook_t *));
    if (tmp != NULL) {
        (*table)->sbooks = tmp;
        (*table)->max_size = (*table)->cur_size;

        return OK;
    }

    return MEM_ALLOC_ERR;
}

void sbook_free(sbook_t *sbook) {
    if (sbook == NULL) {
        return;
    }

    if (sbook->record_book) {
        free(sbook->record_book);
    }

    free(sbook);
}

void sbook_free_table(sbook_table_t *table) {
    if (table == NULL) {
        return;
    }

    for (size_t i = TABLE_START; i < table->max_size; ++i) {
        if (table->sbooks[i]) {
            sbook_free(table->sbooks[i]);
        }
    }

    free(table->sbooks);
    free(table);
}

void sbook_print_info(sbook_t *record) {
    printf("|%12zu|", record->isbn);
    printf("%11s|", record->record_book);
    printf("%02d.%02d.%4d|\n", record->date.day, record->date.month, record->date.year);
}

errors_t sbook_show_info_rb(sbook_table_t *table, const char *record_book) {
    bool found = false;
    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        if (strcmp(table->sbooks[i]->record_book, record_book) == 0) {
            sbook_print_info(table->sbooks[i]);
            found = true;
        }
    }
    if (found == false) {
        return SBOOK_REC_NOT_FOUND;
    }

    return OK;
}

errors_t sbook_show_info_isbn(sbook_table_t *table, const size_t isbn) {
    bool found = false;
    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        if (table->sbooks[i]->isbn == isbn) {
            sbook_print_info(table->sbooks[i]);
            found = true;
        }
    }
    if (found == false) {
        return SBOOK_REC_NOT_FOUND;
    }

    return OK;
}

errors_t sbook_split_date(date_t *date, const char *string) {
    char *cur;
    char tmp_string[INIT_SIZE] = {0};

    strcpy(tmp_string, string);
    cur = strtok(tmp_string, ".");
    if (cur) {
        char *end;
        date->day = strtol(cur, &end, 10);
        if (*end != 0) {
            return CSV_READ_ERR;
        }
        cur = strtok(NULL, ".");
        if (cur) {
            date->month = strtol(cur, &end, 10);
            if (*end != 0) {
                return CSV_READ_ERR;
            }
            cur = strtok(NULL, ".");
            if (cur) {
                date->year = strtol(cur, &end, 10);
                if (*end != 0) {
                    return CSV_READ_ERR;
                }
            }
        }
    }
    return OK;
}

bool sbook_student_has_books(sbook_table_t *table, const char *record_book) {
    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        if (strcmp(table->sbooks[i]->record_book, record_book) == 0) {
            return true;
        }
    }
    return false;
}

void sbook_dump_table(sbook_table_t *table, FILE *outfile) {
    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        fprintf(outfile, "%zu;", table->sbooks[i]->isbn);
        fprintf(outfile, "%s;", table->sbooks[i]->record_book);
        fprintf(outfile, "%02hi.", table->sbooks[i]->date.day);
        fprintf(outfile, "%02hi.", table->sbooks[i]->date.month);
        fprintf(outfile, "%hi\n", table->sbooks[i]->date.year);
    }
}

errors_t backup_sbook_table(sbook_table_t *table) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char filename[INIT_SIZE] = {0};
    snprintf(filename, INIT_SIZE, "student_books_%d-%d-%d_%d_%d_%d.csv", tm.tm_mday, tm.tm_mon, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

    errors_t status = FILE_OPEN_ERR;
    FILE *outfile = fopen(filename, "w");
    if (outfile) {
        sbook_dump_table(table, outfile);
        fclose(outfile);
        status = SBOOK_TABLE_SAVED;
    }
    return status;
}

errors_t sbook_add(sbook_table_t *table, const size_t isbn, const char *record_book) {
    errors_t status;
    sbook_t *sb = sbook_init(&status);
    if (sb == NULL) {
        return status;
    }

    sb->record_book = malloc((strlen(record_book) + 1) * sizeof(char));
    if (sb->record_book == NULL) {
        status = MEM_ALLOC_ERR;
        sbook_free(sb);
        return status;
    }
    strcpy(sb->record_book, record_book);

    sb->isbn = isbn;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    date_t date = {0};
    date.day = tm.tm_mday;
    date.month = tm.tm_mon;
    date.year = tm.tm_year + 1900;

    sb->date = date;

    return sbook_append_table(table, sb);
}

errors_t sbook_append_table(sbook_table_t *table, sbook_t *sb) {
    errors_t status;
    if (table->cur_size + 1 == table->max_size) {
        status = sbook_extend_table(table, 10);
    }

    if (status == OK) {
        table->sbooks[table->cur_size++] = sb;
    }

    return status;
}

errors_t sbook_remove_isbn(sbook_table_t *table, const size_t isbn, const char *record_book) {
    size_t i = 0;
    for (; i < table->cur_size; ++i) {
        if (strcmp(table->sbooks[i]->record_book, record_book) == 0 && table->sbooks[i]->isbn == isbn) {
            break;
        }
    }

    if (i == table->cur_size) {
        return SBOOK_REC_NOT_FOUND;
    }

    sbook_free(table->sbooks[i]);
    table->sbooks[i] = NULL;

    memmove(table->sbooks + i, table->sbooks + i + 1, (table->cur_size - i - 1) * sizeof(sbook_t *));
    --(table->cur_size);

    return OK;
}

// Дата ближайшего возрата. просто обычное сравнивание
void sbook_find_closest_return(sbook_table_t *table, const size_t isbn) {
    date_t date = {0};
    int idx = -1;
    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        if (table->sbooks[i]->isbn == isbn) {
            if (table->sbooks[i]->date.year <= date.year &&
                table->sbooks[i]->date.month <= date.month &&
                table->sbooks[i]->date.day <= date.day) {
                date = table->sbooks[i]->date;
                idx = i;
            }
        }
    }
    if (idx >= 0) {
        printf("Closest return: %02d.%02d.%d\n", date.day, date.month, date.year);
    }
}
