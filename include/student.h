#ifndef STUDENT_H
#define STUDENT_H

#include <stddef.h>

#include "errors.h"

#define STUDENT_TABLE_HEADER "|Record book|      Surname       |        Name        |     Middle name    |       Faculty      |                     Specialty                    |\n"
#define STUDENT_TABLE_DELIM  "|-----------|--------------------|--------------------|--------------------|--------------------|--------------------------------------------------|\n"

#define MAX_RB_LEN 11
#define MAX_SURNAME_LEN 20
#define MAX_NAME_LEN 20
#define MAX_MIDDLE_NAME_LEN 20
#define MAX_FACULTY_LEN 20
#define MAX_SPECIALTY_LEN 50

typedef enum {
    S_RECORD_BOOK,
    S_SURNAME,
    S_NAME,
    S_MIDDLE_NAME,
    S_FACULTY,
    S_SPECIALTY
} read_student_states_t;

typedef struct {
    char *record_book;
    char *surname;
    char *name;
    char *middle_name;
    char *faculty;
    char *specialty;
} student_t;

typedef struct {
    student_t **students;
    size_t cur_size;
    size_t max_size;
} student_table_t;

student_table_t *student_init_table(errors_t *status);
student_t *student_init(errors_t *status);
errors_t student_extend_table(student_table_t *table, const size_t value);

errors_t student_read_row_into_student(const char **row, const int cnt, student_t *student);
student_table_t *student_read_csv(const char *file_path, const char *delim, errors_t *status);

void student_free(student_t *student);
void student_free_table(student_table_t *table);

void student_print_info(student_t *student);
void student_print_table(student_table_t *table);

int student_search_rb(student_table_t *table, const char *record_book);
errors_t student_search_surname(student_table_t *table, const char *surname);

student_t *student_form_student(const char *record_book,
                        const char *surname,
                        const char *name,
                        const char *middle_name,
                        const char *faculty,
                        const char *specialty,
                        errors_t *status);
errors_t student_append_table(student_table_t *table, student_t *student);

errors_t student_add(student_table_t *table);
errors_t student_delete_rb(student_table_t *table, const char *record_book);
errors_t student_show_info_rb(student_table_t *table, const char *record_book);
errors_t student_edit_info_rb(student_table_t *table, const char *record_book);
errors_t give_student_to_student(student_table_t *table, const char *record_book);
errors_t receive_student_from_student(student_table_t *table, const char *record_book);

errors_t student_table_save(student_table_t *table);
errors_t student_table_restore(student_table_t **table, const char* filename);

#endif
