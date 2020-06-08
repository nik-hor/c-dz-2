

#include "student.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "csvparser.h"
#include "io_funcs.h"

void student_init_array_values(student_table_t *st) {
    for (size_t i = st->cur_size; i < st->max_size; ++i) {
        st->students[i] = NULL;
    }
}

student_table_t *student_init_table(errors_t *status) {
    student_table_t *st = NULL;
    st = malloc(sizeof(student_table_t));
    if (st == NULL) {
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    st->students = malloc(INIT_SIZE * sizeof(student_t *));
    if (st->students == NULL) {
        free(st);
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    st->max_size = INIT_SIZE;
    st->cur_size = 0;

    student_init_array_values(st);

    *status = OK;
    return st;
}

errors_t student_read_row_into_student(const char **row, const int cnt, student_t *student) {
    errors_t status = OK;
    for (int i = 0; i < cnt && status == OK; ++i) {
        switch (i) {
            case S_RECORD_BOOK: {
                if (strlen(row[i]) < MAX_RB_LEN) {
                    student->record_book = malloc((strlen(row[i]) + 1) * sizeof(char));
                    if (student->record_book) {
                        strcpy(student->record_book, row[i]);
                    } else {
                        status = MEM_ALLOC_ERR;
                    }
                } else {
                    status = CSV_READ_ERR;
                }
                break;
            }
            case S_SURNAME: {
                if (strlen(row[i]) < MAX_SURNAME_LEN) {
                    student->surname = malloc((strlen(row[i]) + 1) * sizeof(char));
                    if (student->surname) {
                        strcpy(student->surname, row[i]);
                    } else {
                        status = MEM_ALLOC_ERR;
                    }
                } else {
                    status = CSV_READ_ERR;
                }
                break;
            }
            case S_NAME: {
                if (strlen(row[i]) < MAX_NAME_LEN) {
                    student->name = malloc((strlen(row[i]) + 1) * sizeof(char));
                    if (student->name) {
                        strcpy(student->name, row[i]);
                    } else {
                        status = MEM_ALLOC_ERR;
                    }
                } else {
                    status = CSV_READ_ERR;
                }
                break;
            }
            case S_MIDDLE_NAME: {
                if (strlen(row[i]) < MAX_MIDDLE_NAME_LEN) {
                    student->middle_name = malloc((strlen(row[i]) + 1) * sizeof(char));
                    if (student->middle_name) {
                        strcpy(student->middle_name, row[i]);
                    } else {
                        status = MEM_ALLOC_ERR;
                    }
                } else {
                    status = CSV_READ_ERR;
                }
                break;
            }
            case S_FACULTY: {
                if (strlen(row[i]) < MAX_FACULTY_LEN) {
                    student->faculty = malloc((strlen(row[i]) + 1) * sizeof(char));
                    if (student->faculty) {
                        strcpy(student->faculty, row[i]);
                    } else {
                        status = MEM_ALLOC_ERR;
                    }
                } else {
                    status = CSV_READ_ERR;
                }
                break;
            }
            case S_SPECIALTY: {
                if (strlen(row[i]) < MAX_SPECIALTY_LEN) {
                    student->specialty = malloc((strlen(row[i]) + 1) * sizeof(char));
                    if (student->specialty) {
                        strcpy(student->specialty, row[i]);
                    } else {
                        status = MEM_ALLOC_ERR;
                    }
                } else {
                    status = CSV_READ_ERR;
                }
                break;
            }
        }
    }

    return status;
}

student_t *student_init(errors_t *status) {
    student_t *tmp = malloc(sizeof(student_t));
    if (tmp == NULL) {
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    tmp->record_book = NULL;
    tmp->surname = NULL;
    tmp->name = NULL;
    tmp->middle_name = NULL;
    tmp->faculty = NULL;
    tmp->specialty = NULL;

    return tmp;
}

errors_t student_extend_table(student_table_t *table, const size_t value) {
    if (value == 0) {
        return OK;
    }

    student_t **tmp = realloc(table->students, (table->max_size + value) * sizeof(student_t *));
    if (tmp) {
        table->students = tmp;
        student_init_array_values(table);
    } else {
        return MEM_ALLOC_ERR;
    }

    table->max_size += value;

    return OK;
}

student_table_t *student_read_csv(const char *file_path, const char *delim, errors_t *status) {
    student_table_t *student_table = student_init_table(status);
    if (student_table == NULL) {
        return NULL;
    }

    CsvParser *csvparser = init_parser(file_path, delim, status);

    if (*status != OK) {
        student_free_table(student_table);
        return NULL;
    }

    CsvRow *row;
    *status = OK;

    student_t *student = NULL;
    while ((row = get_row(csvparser)) && *status == OK) {
        const char **rowFields = get_fields(row);
        const int cnt = get_num_fields(row);
        student = student_init(status);
        if (student) {
            *status = student_read_row_into_student(rowFields, cnt, student);
            if (*status == OK) {
                destroy_row(row);
                if (student_table->cur_size == student_table->max_size) {
                    *status = student_extend_table(student_table, 50);
                }
                if (*status == OK) {
                    student_table->students[student_table->cur_size] = student;
                    ++(student_table->cur_size);
                }
            }
        } else {
            *status = MEM_ALLOC_ERR;
        }
    }

    printf("Students csv load: %s\n", str_err(csvparser->err));
    free_parser(csvparser);

    if (student && *status != OK) {
        student_free(student);
    }

    return student_table;
}

void student_free(student_t *student) {
    if (student == NULL) {
        return;
    }

    if (student->record_book) {
        free(student->record_book);
    }

    if (student->surname) {
        free(student->surname);
    }

    if (student->name) {
        free(student->name);
    }

    if (student->middle_name) {
        free(student->middle_name);
    }

    if (student->faculty) {
        free(student->faculty);
    }

    if (student->specialty) {
        free(student->specialty);
    }

    free(student);
}

void student_free_table(student_table_t *table) {
    if (table == NULL) {
        return;
    }

    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        if (table->students[i]) {
            student_free(table->students[i]);
        }
    }

    free(table->students);
    free(table);
}

void student_print_info(student_t *student) {
    printf("|%11s|", student->record_book);
    printf("%20s|", student->surname);
    printf("%20s|", student->name);
    printf("%20s|", student->middle_name);
    printf("%20s|", student->faculty);
    printf("%50s|\n", student->specialty);
}

void student_print_table(student_table_t *table) {
    printf(STUDENT_TABLE_HEADER);
    printf(STUDENT_TABLE_DELIM);
    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        student_print_info(table->students[i]);
        printf(STUDENT_TABLE_DELIM);
    }
}

int student_search_rb(student_table_t *table, const char *record_book) {
    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        if (strcmp(table->students[i]->record_book, record_book) == 0) {
            return i;
        }
    }
    return NOT_FOUND;
}

errors_t student_search_surname(student_table_t *table, const char *surname) {
    bool found = false;
    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        if (strcmp(table->students[i]->surname, surname) == 0) {
            student_print_info(table->students[i]);
            found = true;
        }
    }
    if (found == false) {
        return STUDENT_NOT_FOUND;
    }

    return OK;
}

student_t *student_form_student(const char *record_book,
                                const char *surname,
                                const char *name,
                                const char *middle_name,
                                const char *faculty,
                                const char *specialty,
                                errors_t *status) {
    student_t *student = student_init(status);
    if (student == NULL) {
        return NULL;
    }

    student->record_book = malloc((strlen(record_book) + 1) * sizeof(char));
    if (student->record_book == NULL) {
        student_free(student);
        *status = MEM_ALLOC_ERR;
        return NULL;
    }
    strcpy(student->record_book, record_book);

    student->surname = malloc((strlen(surname) + 1) * sizeof(char));
    if (student->surname == NULL) {
        student_free(student);
        *status = MEM_ALLOC_ERR;
        return NULL;
    }
    strcpy(student->surname, surname);

    student->name = malloc((strlen(name) + 1) * sizeof(char));
    if (student->name == NULL) {
        student_free(student);
        *status = MEM_ALLOC_ERR;
        return NULL;
    }
    strcpy(student->name, name);

    student->middle_name = malloc((strlen(middle_name) + 1) * sizeof(char));
    if (student->middle_name == NULL) {
        student_free(student);
        *status = MEM_ALLOC_ERR;
        return NULL;
    }
    strcpy(student->middle_name, middle_name);

    student->faculty = malloc((strlen(faculty) + 1) * sizeof(char));
    if (student->faculty == NULL) {
        student_free(student);
        *status = MEM_ALLOC_ERR;
        return NULL;
    }
    strcpy(student->faculty, faculty);

    student->specialty = malloc((strlen(specialty) + 1) * sizeof(char));
    if (student->specialty == NULL) {
        student_free(student);
        *status = MEM_ALLOC_ERR;
        return NULL;
    }
    strcpy(student->specialty, specialty);

    return student;
}

errors_t student_append_table(student_table_t *table, student_t *student) {
    errors_t status = OK;
    if (table->cur_size + 1 == table->max_size) {
        status = student_extend_table(table, SMALL_EXTEND);
    }

    if (status == OK) {
        table->students[table->cur_size++] = student;
    }

    return status;
}

errors_t student_add(student_table_t *table) {
    printf("Enter record book: ");
    char rb_buf[MAX_RB_LEN + 1];
    errors_t status = OK;
    if ((status = read_string(rb_buf, sizeof(rb_buf), stdin)) != OK) {
        return status;
    }

    if (student_search_rb(table, rb_buf) != NOT_FOUND) {
        return STUDENT_ALREADY_EXISTS;
    }

    printf("Enter surname: ");
    char surname_buf[MAX_SURNAME_LEN + 1];
    if ((status = read_string(surname_buf, sizeof(surname_buf), stdin)) != OK) {
        return status;
    }

    printf("Enter name: ");
    char name_buf[MAX_NAME_LEN + 1];
    if ((status = read_string(name_buf, sizeof(name_buf), stdin)) != OK) {
        return status;
    }

    printf("Enter middle name: ");
    char mn_buf[MAX_MIDDLE_NAME_LEN + 1];
    if ((status = read_string(mn_buf, sizeof(mn_buf), stdin)) != OK) {
        return status;
    }

    printf("Enter faculty: ");
    char faculty_buf[MAX_FACULTY_LEN + 1];
    if ((status = read_string(faculty_buf, sizeof(faculty_buf), stdin)) != OK) {
        return status;
    }

    printf("Enter specialty: ");
    char spec_buf[MAX_SPECIALTY_LEN + 1];
    if ((status = read_string(spec_buf, sizeof(spec_buf), stdin)) != OK) {
        return status;
    }

    student_t *student = student_form_student(rb_buf, surname_buf, name_buf, mn_buf, faculty_buf, spec_buf, &status);
    if (student) {
        status = student_append_table(table, student);
    }

    return status;
}

errors_t student_delete_rb(student_table_t *table, const char *record_book) {
    int stud_index = student_search_rb(table, record_book);
    if (stud_index == NOT_FOUND) {
        return STUDENT_NOT_FOUND;
    }

    student_free(table->students[stud_index]);
    table->students[stud_index] = NULL;

    memmove(table->students + stud_index, table->students + stud_index + 1, (table->cur_size - stud_index - 1) * sizeof(student_t *));
    --(table->cur_size);

    return OK;
}

errors_t student_show_info_rb(student_table_t *table, const char *record_book) {
    int stud_index = student_search_rb(table, record_book);
    if (stud_index == NOT_FOUND) {
        return STUDENT_NOT_FOUND;
    }

    student_print_info(table->students[stud_index]);

    return OK;
}

errors_t student_edit_info_rb(student_table_t *table, const char *record_book) {
    int stud_index = student_search_rb(table, record_book);
    if (stud_index == NOT_FOUND) {
        return STUDENT_NOT_FOUND;
    }

    printf("Enter new surname: ");
    errors_t status;
    char surname_buf[MAX_SURNAME_LEN + 1];
    if ((status = read_string(surname_buf, sizeof(surname_buf), stdin)) != OK) {
        return status;
    }

    printf("Enter new name: ");
    char name_buf[MAX_NAME_LEN + 1];
    if ((status = read_string(name_buf, sizeof(name_buf), stdin)) != OK) {
        return status;
    }

    printf("Enter new middle name: ");
    char mn_buf[MAX_MIDDLE_NAME_LEN + 1];
    if ((status = read_string(mn_buf, sizeof(mn_buf), stdin)) != OK) {
        return status;
    }

    printf("Enter new faculty: ");
    char faculty_buf[MAX_FACULTY_LEN + 1];
    if ((status = read_string(faculty_buf, sizeof(faculty_buf), stdin)) != OK) {
        return status;
    }

    printf("Enter new specialty: ");
    char spec_buf[MAX_SPECIALTY_LEN + 1];
    if ((status = read_string(spec_buf, sizeof(spec_buf), stdin)) != OK) {
        return status;
    }

    student_t *student = student_form_student(record_book, surname_buf, name_buf, mn_buf, faculty_buf, spec_buf, &status);
    if (student) {
        student_free(table->students[stud_index]);
        table->students[stud_index] = student;
    }

    return status;
}

void student_dump_table(student_table_t *table, FILE *outfile) {
    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        fprintf(outfile, "%s;", table->students[i]->record_book);
        fprintf(outfile, "%s;", table->students[i]->surname);
        fprintf(outfile, "%s;", table->students[i]->name);
        fprintf(outfile, "%s;", table->students[i]->middle_name);
        fprintf(outfile, "%s;", table->students[i]->faculty);
        fprintf(outfile, "%s\n", table->students[i]->specialty);
    }
}

errors_t student_table_save(student_table_t *table) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char filename[INIT_SIZE] = {0};
    snprintf(filename, INIT_SIZE, "students_%d-%d-%d_%d_%d_%d.csv", tm.tm_mday, tm.tm_mon, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

    errors_t status = FILE_OPEN_ERR;
    FILE *outfile = fopen(filename, "w");
    if (outfile) {
        student_dump_table(table, outfile);
        fclose(outfile);
        status = STUDENT_TABLE_SAVED;
    }
    return status;
}
errors_t student_table_restore(student_table_t **table, const char *filename) {
    errors_t status;
    student_table_t *bt = student_read_csv(filename, ";", &status);
    if (bt) {
        student_free_table(*table);
        *table = bt;
    }
    return status;
}
