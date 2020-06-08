#include "errors.h"

#include <stdio.h>

// Выводит код ошибки на жкран
void raise_error(errors_t status) {
    puts(str_err(status));
}

// возвращает текстовое описание кода ошибки
char* str_err(errors_t status) {
    switch (status) {
        case OK:
            return "Success";
            break;
        case MEM_ALLOC_ERR:
            return "Memmory alloc err";
            break;
        case READ_ERR:
            return "Read input err";
            break;
        case LOGGER_SETUP_ERR:
            return "Logger setup error";
            break;

        case NULL_FILE_PATH:
            return "Csv file path is NULL";
            break;
        case UNSUPPORTED_DELIM:
            return "Unsuported delimiter in parser init";
            break;

        case BOOK_NOT_FOUND:
            return "Book not found";
            break;
        case STUDENT_NOT_FOUND:
            return "Student not found";
            break;
        case USER_NOT_FOUND:
            return "User not found";
            break;
        case SBOOK_REC_NOT_FOUND:
            return "No student books records was found";
            break;

        case STRING_OVERFLOW_ERR:
            return "String oerflow";
            break;
        case STRING_READ_ERR:
            return "String read err";
            break;

        case READ_BOOK_ISBN_ERR:
            return "ISBN read err";
            break;
        case READ_BOOK_COUNT_ERR:
            return "Book count read err";
            break;
        case READ_BOOK_AVAILABLE_ERR:
            return "Book available read err";
            break;

        case BOOK_ALREADY_EXISTS:
            return "Book already exists";
            break;
        case STUDENT_ALREADY_EXISTS:
            return "Student already exists";
            break;

        case NO_AVAILABLE_BOOKS:
            return "No available books";
            break;
        case CANT_RETURN:
            return "Cant return book";
            break;

        case INCORRECT_PASS:
            return "Wrong pass";
            break;
        case INCORRECT_CHOICE:
            return "Incorrect menu choice";
            break;

        case BOOK_TABLE_SAVED:
            return "Book table saved";
            break;
        case STUDENT_TABLE_SAVED:
            return "Student table saved";
            break;
        case SBOOK_TABLE_SAVED:
            return "Student books table saved";
            break;

        case STUDENT_HAS_BOOKS:
            return "Student has books";
            break;

        case FILE_OPEN_ERR:
            return "File open err";
            break;
        case CSV_READ_ERR:
            return "Invalid csv file";
            break;
    }
    return "";
}