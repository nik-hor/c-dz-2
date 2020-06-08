#include "books.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "csvparser.h"
#include "io_funcs.h"

// Инициализация новых значений массива
void book_init_array_values(book_table_t *bt) {
    for (size_t i = bt->cur_size; i < bt->max_size; ++i) {
        bt->books[i] = NULL;
    }
}

// Инициализация таблицы
book_table_t *book_init_table(errors_t *status) {
    book_table_t *bt = NULL;
    bt = malloc(sizeof(book_table_t));
    if (bt == NULL) {
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    bt->books = malloc(INIT_SIZE * sizeof(book_t *));
    if (bt->books == NULL) {
        free(bt);
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    bt->max_size = INIT_SIZE;
    bt->cur_size = 0;

    book_init_array_values(bt);

    *status = OK;
    return bt;
}

// Считываем строку csv файла в структуру
errors_t book_read_row_into_book(const char **row, const int cnt, book_t *book) {
    errors_t status = OK;
    for (int i = 0; i < cnt && status == OK; ++i) {
        switch (i) {
            case B_ISBN: {
                char *end;
                book->isbn = strtoul(row[i], &end, 10);
                if (*end != 0) {
                    status = CSV_READ_ERR;
                }
                break;
            }
            case B_AUTHORS_NAME: {
                if (strlen(row[i]) < AUTHOR_MAX_LEN) {
                    book->authors_name = malloc((strlen(row[i]) + 1) * sizeof(char));
                    if (book->authors_name) {
                        strcpy(book->authors_name, row[i]);
                    } else {
                        status = MEM_ALLOC_ERR;
                    }
                } else {
                    status = CSV_READ_ERR;
                }
                break;
            }
            case B_BOOK_NAME: {
                if (strlen(row[i]) < BOOK_NAME_MAX_LEN) {
                    book->book_name = malloc((strlen(row[i]) + 1) * sizeof(char));
                    if (book->book_name) {
                        strcpy(book->book_name, row[i]);
                    } else {
                        status = MEM_ALLOC_ERR;
                    }
                } else {
                    status = CSV_READ_ERR;
                }
                break;
            }
            case B_TOTAL: {
                char *end;
                book->total = strtoul(row[i], &end, 10);
                if (*end != 0) {
                    status = CSV_READ_ERR;
                }
                break;
            }
            case B_AVAILABLE: {
                char *end;
                book->available = strtoul(row[i], &end, 10);
                if (*end != 0) {
                    status = CSV_READ_ERR;
                }
                break;
            }
        }
    }

    return status;
}

// Инициализация одно экземпляра книги
book_t *book_init(errors_t *status) {
    book_t *tmp = malloc(sizeof(book_t));
    if (tmp == NULL) {
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    tmp->authors_name = NULL;
    tmp->book_name = NULL;

    return tmp;
}

// Расширение таблицы
errors_t book_extend_table(book_table_t *table, const size_t value) {
    if (value == 0) {
        return OK;
    }

    book_t **tmp = realloc(table->books, (table->max_size + value) * sizeof(book_t *));
    if (tmp) {
        table->books = tmp;
        book_init_array_values(table);
    } else {
        return MEM_ALLOC_ERR;
    }

    table->max_size += value;

    return OK;
}

// Чтение файла в таблицу
book_table_t *book_read_csv(const char *file_path, const char *delim, errors_t *status) {
    book_table_t *book_table = book_init_table(status);
    if (book_table == NULL) {
        return NULL;
    }

    // Инициализируем парсер
    CsvParser *csvparser = init_parser(file_path, delim, status);
    if (*status != OK) {
        book_table_free(book_table);
        return NULL;
    }

    CsvRow *row;
    *status = OK;

    book_t *book = NULL;
    while ((row = get_row(csvparser)) && *status == OK) {  // Пока файл не закончился или не случилась ошибка
        const char **rowFields = get_fields(row);          // Получаем очередную строку
        const int cnt = get_num_fields(row);               // Получаем количество полей строки
        book = book_init(status);                          // Инициализируем экземпляр книги (в неё будем читать)
        if (book) {
            *status = book_read_row_into_book(rowFields, cnt, book);  // Считываем данные
            if (*status == OK) {
                destroy_row(row);                                    // Освобождаем память из под считанной строки
                if (book_table->cur_size == book_table->max_size) {  // Если у нас не хватает места - расширяем таблицу
                    *status = book_extend_table(book_table, HUGE_EXTEND);
                }
                if (*status == OK) {
                    book_table->books[book_table->cur_size] = book;  // Заносим экземпляр в таблицу
                    ++(book_table->cur_size);
                }
            }
        } else {
            *status = MEM_ALLOC_ERR;
        }
    }

    printf("Books csv load: %s\n", str_err(csvparser->err));  // Выводим успех/ошибку парсера
    free_parser(csvparser);                                   // Деиниицализируем парсер

    if (book && *status != OK) {  // Если произошла ошибка - освобождаем память
        book_free(book);
    }
    book_table_sort(book_table);  // Сортируем (по условию)

    return book_table;
}

// Освобождение памяти из под книги
void book_free(book_t *book) {
    if (book == NULL) {
        return;
    }

    if (book->authors_name) {
        free(book->authors_name);
    }

    if (book->book_name) {
        free(book->book_name);
    }

    free(book);
}

// Освобождение памяти из под таблицы
void book_table_free(book_table_t *table) {
    if (table == NULL) {
        return;
    }

    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        if (table->books[i]) {
            book_free(table->books[i]);
        }
    }

    free(table->books);
    free(table);
}

// Обмен данными между двумя указателями на указатель
void book_swap(book_t **x, book_t **y) {
    book_t *temp = *x;
    *x = *y;
    *y = temp;
}

// Сортировка - пузырёк с флагом
void book_table_sort(book_table_t *table) {
    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        int skip = 1;
        for (size_t j = table->cur_size - 1; j > i; --j) {
            if (table->books[j - 1] < table->books[j]) {
                book_swap(&table->books[j], &table->books[j - 1]);
                skip = 0;
            }
        }
        if (skip)
            break;
    }
}

void book_print_info(book_t *book) {
    printf("|%12zu|", book->isbn);
    printf("%25s|", book->authors_name);
    printf("%25s|", book->book_name);
    printf("%5zu|", book->total);
    printf("%9zu|\n", book->available);
}

void book_print_table(book_table_t *table) {
    printf(BOOK_TABLE_HEADER);
    printf(BOOK_TABLE_DELIM);
    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        book_print_info(table->books[i]);
        printf(BOOK_TABLE_DELIM);
    }
}

// Рекурсивный бинарный поиск (таблица упорядоченна). Рекурсия хвостовая (вызов является последней операцией перед возвратом из функции)
int book_search_isbn(book_table_t *table, int low, int high, const size_t isbn) {
    if (high < low) {
        return NOT_FOUND;
    }

    int mid = (low + high) / 2;

    if (table->books[mid]->isbn == isbn) {
        return mid;
    }
    if (table->books[mid]->isbn > isbn) {
        return book_search_isbn(table, (mid + 1), high, isbn);
    }

    return book_search_isbn(table, low, (mid - 1), isbn);
}

// Поиск книги по автору
errors_t book_search_author(book_table_t *table, const char *author_name) {
    bool found = false;
    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        if (strcmp(table->books[i]->authors_name, author_name) == 0) {
            book_print_info(table->books[i]);
            found = true;
        }
    }
    if (found == false) {
        return BOOK_NOT_FOUND;
    }

    return OK;
}

// "Сборщик" экземпляра книги из отдельных данных
book_t *book_form_book(const size_t isbn, 
                       const char *auth_name, 
                       const char *name, 
                       const size_t total, 
                       const size_t available, 
                       errors_t *status) {
    book_t *book = book_init(status);
    if (book == NULL) {
        return NULL;
    }

    book->authors_name = malloc((strlen(auth_name) + 1) * sizeof(char));
    if (book->authors_name == NULL) {
        book_free(book);
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    strcpy(book->authors_name, auth_name);

    book->book_name = malloc((strlen(name) + 1) * sizeof(char));
    if (book->book_name == NULL) {
        book_free(book);
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    strcpy(book->book_name, name);

    book->isbn = isbn;
    book->total = total;
    book->available = available;

    return book;
}

// Добавить книгу в конец таблицы
errors_t book_append_table(book_table_t *table, book_t *book) {
    errors_t status;
    if (table->cur_size + 1 == table->max_size) {
        status = book_extend_table(table, SMALL_EXTEND);
    }

    if (status == OK) {
        table->books[table->cur_size++] = book;
    }

    return status;
}

// Чтение книги с потока ввода
errors_t book_add(book_table_t *table) {
    printf("Enter isbn: ");
    size_t isbn;
    if (read_size_t(&isbn) != OK) {
        return READ_BOOK_ISBN_ERR;
    }

    if (book_search_isbn(table, TABLE_START, table->cur_size, isbn) != NOT_FOUND) {
        return BOOK_ALREADY_EXISTS;
    }

    printf("Enter authors: ");
    char auth_buf[AUTHOR_MAX_LEN + 1];  // Здесь и далее +1 нужно для fgets (он читает вместе с \n)
    errors_t status;
    if ((status = read_string(auth_buf, sizeof(auth_buf), stdin)) != OK) {
        return status;
    }

    printf("Enter name: ");
    char name_buf[BOOK_NAME_MAX_LEN + 1];
    if ((status = read_string(name_buf, sizeof(name_buf), stdin)) != OK) {
        return status;
    }

    printf("Enter total count: ");
    size_t total;
    if (read_size_t(&total) != OK) {
        return READ_BOOK_COUNT_ERR;
    }

    printf("Enter available: ");
    size_t available;
    if (read_size_t(&available) != OK) {
        return READ_BOOK_AVAILABLE_ERR;
    }

    book_t *book = book_form_book(isbn, auth_buf, name_buf, total, isbn, &status);
    if (book) {
        status = book_append_table(table, book);
    }

    return status;
}

errors_t book_delete(book_table_t *table, const size_t isbn) {
    int book_index = book_search_isbn(table, TABLE_START, table->cur_size, isbn);
    if (book_index == NOT_FOUND) {
        return BOOK_NOT_FOUND;
    }

    if (table->books[book_index]->available != table->books[book_index]->total) {
        return STUDENT_HAS_BOOKS;
    }

    book_free(table->books[book_index]);
    table->books[book_index] = NULL;

    // Здесь мы двигаем весь массив на 1 позицию влево (на место удалённой книги)
    // memmove(куда, откуда, сколько байт)
    // table->books + book_index - даст адрес удалённой книги
    // table->books[book_index + 1] - даст адрес книги справа от удалённой
    // (table->cur_size - book_index - 1) * sizeof(book_t *) - получим количесво элементов, которые нужно сдвинуть * размер указателя на структуру
    memmove(table->books + book_index, table->books + book_index + 1, (table->cur_size - book_index - 1) * sizeof(book_t *));
    --(table->cur_size);

    return OK;
}

// Показать информацию о книге по номеру
errors_t book_show_info(book_table_t *table, const size_t isbn) {
    int book_index = book_search_isbn(table, TABLE_START, table->cur_size, isbn);
    if (book_index == NOT_FOUND) {
        return BOOK_NOT_FOUND;
    }

    book_print_info(table->books[book_index]);

    return OK;
}

// Редактировать книгу
errors_t book_edit_info_isbn(book_table_t *table, const size_t isbn) {
    int book_index = book_search_isbn(table, TABLE_START, table->cur_size, isbn);
    if (book_index == NOT_FOUND) {
        return BOOK_NOT_FOUND;
    }

    printf("Enter new authors: ");
    char auth_buf[AUTHOR_MAX_LEN + 1];
    errors_t status;
    if ((status = read_string(auth_buf, sizeof(auth_buf), stdin)) != OK) {
        return status;
    }

    printf("Enter new name: ");
    char name_buf[BOOK_NAME_MAX_LEN + 1];
    if ((status = read_string(name_buf, sizeof(name_buf), stdin)) != OK) {
        return status;
    }

    printf("Enter new total count: ");
    size_t total;
    if (read_size_t(&total) != OK) {
        return READ_BOOK_COUNT_ERR;
    }

    printf("Enter new available: ");
    size_t available;
    if (read_size_t(&available) != OK ) {
        return READ_BOOK_AVAILABLE_ERR;
    }

    book_t *book = book_form_book(isbn, auth_buf, name_buf, total, available, &status);
    if (book) {
        book_free(table->books[book_index]);
        table->books[book_index] = book;
    }

    return status;
}

// Отредактировать общее количество книг в библиотеке
errors_t book_edit_total_isbn(book_table_t *table, const size_t isbn, const size_t total) {
    int book_index = book_search_isbn(table, TABLE_START, table->cur_size, isbn);
    if (book_index == NOT_FOUND) {
        return BOOK_NOT_FOUND;
    }

    table->books[book_index]->total = total;
    return OK;
}

// Выдать книгу студенту
errors_t book_give_to_student_isbn(book_table_t *table, const size_t isbn) {
    int book_index = book_search_isbn(table, TABLE_START, table->cur_size, isbn);
    if (book_index == NOT_FOUND) {  // Если не найдена
        return BOOK_NOT_FOUND;
    }

    if (table->books[book_index]->available == 0) {  // Если нельзя выдать
        return NO_AVAILABLE_BOOKS;
    }

    --(table->books[book_index]->available);  // уменьшить количество

    return OK;
}

// Вернуть книгу
errors_t book_receive_from_student_isbn(book_table_t *table, const size_t isbn) {
    int book_index = book_search_isbn(table, TABLE_START, table->cur_size, isbn);
    if (book_index == NOT_FOUND) {  // Если найдена
        return BOOK_NOT_FOUND;
    }

    if (table->books[book_index]->available == table->books[book_index]->total) {  // Если есть что возвращать
        return CANT_RETURN;
    }

    ++(table->books[book_index]->available);

    return OK;
}

// Сброс таблицы в файл формата c csv
void book_dump_table(book_table_t *table, FILE *outfile) {
    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        fprintf(outfile, "%zu;", table->books[i]->isbn);
        fprintf(outfile, "%s;", table->books[i]->authors_name);
        fprintf(outfile, "%s;", table->books[i]->book_name);
        fprintf(outfile, "%zu;", table->books[i]->total);
        fprintf(outfile, "%zu\n", table->books[i]->available);
    }
}

// Backup
errors_t book_table_save(book_table_t *table) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char filename[INIT_SIZE] = {0};  // +1900 - Такие правила у tm (начало отсчёта)
    snprintf(filename, INIT_SIZE, "books_%d-%d-%d_%d_%d_%d.csv", tm.tm_mday, tm.tm_mon, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

    errors_t status = FILE_OPEN_ERR;
    FILE *outfile = fopen(filename, "w");
    if (outfile) {
        book_dump_table(table, outfile);
        fclose(outfile);
        status = BOOK_TABLE_SAVED;
    }
    return status;
}

// Restore
errors_t book_table_restore(book_table_t **table, const char* filename) {
    errors_t status;
    book_table_t *bt = book_read_csv(filename, ";", &status);
    if (bt && status == OK) {
        book_table_free(*table);
        *table = bt;
    }
    return status;
}
