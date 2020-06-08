#ifndef ERRORS_H
#define ERRORS_H

// Индекс неудачного поиска
#define NOT_FOUND -1

// Начальный размер каждой таблицы
#define INIT_SIZE 100

// Констана расширения таблицы на маленькое количество
#define SMALL_EXTEND 10

// Кожффициент увеличения малой константы
#define EXTEND_MULTIPLIER 5

// Константа расширения таблицы на большое количество
#define HUGE_EXTEND SMALL_EXTEND * EXTEND_MULTIPLIER

// Индекс начала любой таблицы
#define TABLE_START 0

// Все возможные ошибки
typedef enum {
    OK,
    MEM_ALLOC_ERR,
    READ_ERR,
    LOGGER_SETUP_ERR,

    NULL_FILE_PATH,
    UNSUPPORTED_DELIM,

    BOOK_NOT_FOUND,
    STUDENT_NOT_FOUND,
    USER_NOT_FOUND,
    SBOOK_REC_NOT_FOUND,

    STRING_OVERFLOW_ERR,
    STRING_READ_ERR,

    READ_BOOK_ISBN_ERR,
    READ_BOOK_COUNT_ERR,
    READ_BOOK_AVAILABLE_ERR,

    BOOK_ALREADY_EXISTS,
    STUDENT_ALREADY_EXISTS,

    NO_AVAILABLE_BOOKS,
    CANT_RETURN,

    INCORRECT_PASS,
    INCORRECT_CHOICE,

    BOOK_TABLE_SAVED,
    STUDENT_TABLE_SAVED,
    SBOOK_TABLE_SAVED,

    STUDENT_HAS_BOOKS,

    FILE_OPEN_ERR,
    CSV_READ_ERR
} errors_t;

void raise_error(errors_t status);
char* str_err(errors_t status);

#endif
