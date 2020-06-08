#ifndef STUDENT_BOOKS_H
#define STUDENT_BOOKS_H

#include <stdbool.h>
#include <stddef.h>

#include "errors.h"

#define MAX_RB_LEN 11

typedef enum {
    SB_ISBN,
    SB_RECORD_BOOK,
    SB_DATE
} read_sbook_states_t;

typedef struct {
    short day;
    short month;
    short year;
} date_t;

typedef struct {
    size_t isbn;
    char *record_book;
    date_t date;
} sbook_t;

typedef struct {
    sbook_t **sbooks;
    size_t cur_size;
    size_t max_size;
} sbook_table_t;

sbook_table_t *sbook_init_table(errors_t *status);
sbook_t *sbook_init(errors_t *status);
errors_t sbook_extend_table(sbook_table_t *table, const size_t value);

errors_t sbook_read_row_into_sbook(const char **row, const int cnt, sbook_t *sbook);
sbook_table_t *sbook_read_csv(const char *file_path, const char *delim, errors_t *status);

errors_t compress_sbook_table(sbook_table_t **table);

void sbook_free(sbook_t *sbook);
void sbook_free_table(sbook_table_t *table);

void sbook_print_info(sbook_t *record);
errors_t sbook_show_info_rb(sbook_table_t *table, const char *record_book);
errors_t sbook_show_info_isbn(sbook_table_t *table, const size_t isbn);

errors_t sbook_split_date(date_t *date, const char *string);
bool sbook_student_has_books(sbook_table_t *table, const char *record_book);
errors_t backup_sbook_table(sbook_table_t *table);
errors_t sbook_add(sbook_table_t *table, const size_t isbn, const char *record_book);
errors_t sbook_append_table(sbook_table_t *table, sbook_t *sb);
errors_t sbook_remove_isbn(sbook_table_t *table, const size_t isbn, const char* record_book);
void sbook_find_closest_return(sbook_table_t *table, const size_t isbn);

#endif
