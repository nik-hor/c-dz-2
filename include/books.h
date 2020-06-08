#ifndef BOOKS_H
#define BOOKS_H

#include <stddef.h>

#include "errors.h"

// Заголовки для печати таблицы в консоль
#define BOOK_TABLE_HEADER "|    ISBN    |         Authors         |        Book name        |Total|Available|\n"
#define BOOK_TABLE_DELIM  "|------------|-------------------------|-------------------------|-----|---------|\n"

#define AUTHOR_MAX_LEN 25
#define BOOK_NAME_MAX_LEN 25

// Состояния чтения (чтобы было понятно на каком этапе чтения)
typedef enum {
    B_ISBN,
    B_AUTHORS_NAME,
    B_BOOK_NAME,
    B_TOTAL,
    B_AVAILABLE
} read_book_states_t;

typedef struct {
    size_t isbn;
    char *authors_name;
    char *book_name;
    size_t total;
    size_t available;
} book_t;

typedef struct {
    book_t **books;
    size_t cur_size;
    size_t max_size;
} book_table_t;

book_table_t *book_init_table(errors_t *status);
book_t *book_init(errors_t *status);
errors_t book_extend_table(book_table_t *table, const size_t value);

errors_t book_read_row_into_book(const char **row, const int cnt, book_t *book);
book_table_t *book_read_csv(const char *file_path, const char *delim, errors_t *status);

void book_free(book_t *book);
void book_table_free(book_table_t *table);

void book_swap(book_t **x, book_t **y);
void book_table_sort(book_table_t *table);

void book_print_info(book_t *book);
void book_print_table(book_table_t *table);

int book_search_isbn(book_table_t *table, int low, int high, const size_t isbn);
errors_t book_search_author(book_table_t *table, const char *author_name);

book_t *book_form_book(const size_t isbn, const char *auth_name, const char *name, const size_t total, const size_t available, errors_t *status);
errors_t book_append_table(book_table_t *table, book_t *book);

errors_t book_add(book_table_t *table);
errors_t book_delete(book_table_t *table, const size_t isbn);
errors_t book_show_info(book_table_t *table, const size_t isbn);
errors_t book_edit_info_isbn(book_table_t *table, const size_t isbn);
errors_t book_edit_total_isbn(book_table_t *table, const size_t isbn, const size_t total);
errors_t book_give_to_student_isbn(book_table_t *table, const size_t isbn);
errors_t book_receive_from_student_isbn(book_table_t *table, const size_t isbn);

errors_t book_table_save(book_table_t *table);
errors_t book_table_restore(book_table_t **table, const char* filename);

#endif
