#ifndef DB_EXECUTOR_H
#define DB_EXECUTOR_H

#include "books.h"
#include "errors.h"
#include "student.h"
#include "student_books.h"
#include "users.h"
#include "io_funcs.h"

// Поддерживаемые действия
typedef enum {
    // Начальная точка меню
    START_MENU = -2,

    // Выход из программы
    EXIT,

    // Возврат в менб верхнего уровня
    RETURN,

    // Операция с книгами
    BOOK_ADD,
    BOOK_REMOVE,
    BOOK_SHOW_INFO,
    BOOK_PRINT_TABLE,
    BOOK_EDIT,
    BOOK_EDIT_TOTAL,
    BOOK_GIVE,
    BOOK_RECEIVE,
    BOOK_TABLE_SAVE,
    BOOK_TABLE_RESTORE,
    BOOK_SEARCH,
    SHOW_BOOKS_STUDENT,

    // Опреации со студентами
    STUDENT_ADD,
    STUDENT_REMOVE,
    STUDENT_EDIT,
    STUDENT_SHOW_INFO,
    STUDENT_TABLE_SAVE,
    STUDENT_TABLE_RESTORE,
    STUDENT_SEARCH,
    SHOW_STUDENT_BOOKS,

    // конеч меню
    END_MENU
} supported_actions_t;

// Возможные состояния меню у пользователя
typedef enum {
    TOP_LEVEL_MENU,
    BOOK_MENU,
    STUDENT_MENU,
    NO_MENU
} menu_states_t;

// Сам модуль управления
// Содержит в себе все таблицы
// Указатель на текущего пользоваться
// Текущее меню
// И признак полного доступа
typedef struct {
    book_table_t* bt;
    student_table_t* st;
    user_table_t* ut;
    sbook_table_t* sb;
    user_t* cur_user; // текущий пользователь
    menu_states_t menu; // текущее меню
    bool full_access; // признак доступа
} db_executor_t;

db_executor_t* db_init(errors_t* status);
errors_t db_login(db_executor_t* db, const char* login, const char* pass);
void db_destroy(db_executor_t* db);

void db_print_menu(db_executor_t* db);
errors_t db_read_action(db_executor_t* db, int* action);
errors_t db_execute_action(db_executor_t* db, int action);

char* str_action(int action);

#endif
