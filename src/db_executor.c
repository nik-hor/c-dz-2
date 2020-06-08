#include "db_executor.h"

#include <stdio.h>
#include <stdlib.h>

#include "logger.h"

// Инициализация модуля управления
db_executor_t* db_init(errors_t* status) {
    db_executor_t* db = malloc(sizeof(db_executor_t));
    if (db == NULL) {
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    db->bt = NULL;
    db->st = NULL;
    db->ut = NULL;
    db->sb = NULL;
    db->menu = NO_MENU;

    db->bt = book_read_csv("csv_data/books.csv", ";", status);
    if (*status != OK) {
        printf("Books csv load error!\n");
        db_destroy(db);
        return NULL;
    }

    db->st = student_read_csv("csv_data/students.csv", ";", status);
    if (*status != OK) {
        printf("Students csv load error!\n");
        db_destroy(db);
        return NULL;
    }

    db->ut = user_read_csv("csv_data/users.csv", ";", status);
    if (*status != OK) {
        printf("Users csv load error!\n");
        db_destroy(db);
        return NULL;
    }

    db->sb = sbook_read_csv("csv_data/student_books.csv", ";", status);

    if (*status != OK) {
        printf("Student books csv load error!\n");
        db_destroy(db);
        return NULL;
    }

    return db;
}

errors_t db_login(db_executor_t* db, const char* login, const char* pass) {
    errors_t status;
    db->cur_user = user_login_into_sysytem(db->ut, login, pass, &status);
    return status;
}

void db_destroy(db_executor_t* db) {
    if (db->bt) {
        book_table_free(db->bt);
    }
    if (db->st) {
        student_free_table(db->st);
    }
    if (db->ut) {
        user_free_table(db->ut);
    }
    if (db->sb) {
        sbook_free_table(db->sb);
    }
    free(db);
}

void db_print_menu(db_executor_t* db) {
    switch (db->menu) {
        case NO_MENU:
            if (db->cur_user->book_access && db->cur_user->stud_access) {
                db->full_access = true;
                db->menu = TOP_LEVEL_MENU;
                top_level_menu();
                return;
            }

            if (db->cur_user->book_access) {
                db->menu = BOOK_MENU;
                book_menu(db->full_access);
                return;
            }

            if (db->cur_user->stud_access) {
                db->menu = STUDENT_MENU;
                student_menu(db->full_access);
                return;
            }
            break;
        case TOP_LEVEL_MENU:
            top_level_menu();
            break;
        case BOOK_MENU:
            book_menu(db->full_access);
            break;
        case STUDENT_MENU:
            student_menu(db->full_access);
            break;
    }
}

errors_t db_read_action(db_executor_t* db, int* action) {
    printf("Action: ");

    if (read_int(action) != OK) {  // удалось считать
        return READ_ERR;
    }

    if (*action >= END_MENU || *action <= START_MENU) {  // если номер оносится к меню
        return INCORRECT_CHOICE;
    }

    if (*action == RETURN && !db->full_access) {  // если пользователь имеет права, чтобы выйти в меню верхнего уровня
        return INCORRECT_CHOICE;
    }

    if (*action > RETURN && *action < STUDENT_ADD && db->menu != BOOK_MENU) {
        return INCORRECT_CHOICE;
    }

    if (*action > SHOW_BOOKS_STUDENT && *action < END_MENU && db->menu != STUDENT_MENU) {
        return INCORRECT_CHOICE;
    }

    if (*action > RETURN && *action < STUDENT_ADD && !db->cur_user->book_access) {  // если пользователь имеет доступ к меню книг
        return INCORRECT_CHOICE;
    }

    if (*action > SHOW_BOOKS_STUDENT && *action < END_MENU && !db->cur_user->stud_access) {  // имеет доступ к меню студентов
        return INCORRECT_CHOICE;
    }

    return OK;
}

// Обработчик действий
errors_t db_execute_action(db_executor_t* db, int action) {
    errors_t status = OK;
    extern FILE* log_file;
    log_set_fp(log_file);
    switch (action) {
        case EXIT: {
            status = book_table_save(db->bt);
            raise_error(status);
            log_action("Book table backup: %s", str_err(status));

            status = student_table_save(db->st);
            log_action("Student table backup: %s", str_err(status));
            raise_error(status);

            status = backup_sbook_table(db->sb);
            log_action("Student books table backup: %s", str_err(status));
            raise_error(status);

            status = OK;
            break;
        }
        case RETURN: {
            db->menu = TOP_LEVEL_MENU;
            break;
        }
        case BOOK_ADD: {
            status = book_add(db->bt);
            break;
        }
        case BOOK_REMOVE: {
            printf("Enter isbn: ");
            size_t isbn;
            if (read_size_t(&isbn) != OK) {
                return READ_BOOK_ISBN_ERR;
            }
            status = book_delete(db->bt, isbn);
            log_action("With args: %zu", isbn);  // здесь и далее логирование
            break;
        }
        case BOOK_SHOW_INFO: {
            printf("Enter isbn: ");
            size_t isbn;
            if (read_size_t(&isbn) != OK) {
                return READ_BOOK_ISBN_ERR;
            }
            status = book_show_info(db->bt, isbn);
            log_action("With args: %zu", isbn);
            break;
        }
        case BOOK_PRINT_TABLE: {
            book_print_table(db->bt);
            status = OK;
            break;
        }
        case BOOK_EDIT: {
            printf("Enter isbn: ");
            size_t isbn;
            if (read_size_t(&isbn) != OK) {
                return READ_BOOK_ISBN_ERR;
            }
            status = book_edit_info_isbn(db->bt, isbn);
            log_action("With args: %zu", isbn);
            break;
        }
        case BOOK_EDIT_TOTAL: {
            printf("Enter isbn and new total: ");
            size_t isbn;
            if (read_size_t(&isbn) != OK) {
                return READ_ERR;
            }

            size_t total;
            if (read_size_t(&total) != OK) {
                return READ_ERR;
            }

            status = book_edit_total_isbn(db->bt, isbn, total);
            log_action("With args: isbn = %zu and total = %zu", isbn, total);
            break;
        }
        case BOOK_GIVE: {
            printf("Enter isbn: ");
            size_t isbn;
            if (read_size_t(&isbn) != OK) {
                return READ_ERR;
            }

            char rb_buf[MAX_RB_LEN + 1];
            status = read_string(rb_buf, sizeof(rb_buf), stdin);
            if (status == OK) {
                status = book_give_to_student_isbn(db->bt, isbn);
                if (status == OK) {
                    status = sbook_add(db->sb, isbn, rb_buf);
                } else if (status == NO_AVAILABLE_BOOKS) {
                    sbook_find_closest_return(db->sb, isbn);
                }
            }
            log_action("With args: isbn = %zu to student = %s", isbn, rb_buf);
            break;
        }
        case BOOK_RECEIVE: {
            printf("Enter isbn: ");
            size_t isbn;
            if (read_size_t(&isbn) != OK) {
                return READ_ERR;
            }

            char rb_buf[MAX_RB_LEN + 1];
            status = read_string(rb_buf, sizeof(rb_buf), stdin);
            if (status == OK) {
                status = book_receive_from_student_isbn(db->bt, isbn);
                if (status == OK) {
                    status = sbook_remove_isbn(db->sb, isbn, rb_buf);
                }
            }
            log_action("With args: isbn = %zu to student = %s", isbn, rb_buf);
            break;
        }
        case BOOK_TABLE_SAVE: {
            status = book_table_save(db->bt);
            break;
        }
        case BOOK_TABLE_RESTORE: {
            printf("Enter filename: ");
            char filename[INIT_SIZE];
            errors_t status = read_string(filename, INIT_SIZE, stdin);
            if (status == OK) {
                status = book_table_restore(&db->bt, filename);
            }
            break;
        }
        case BOOK_SEARCH: {
            printf("Enter author: ");
            char auth_buf[AUTHOR_MAX_LEN + 1];
            status = read_string(auth_buf, sizeof(auth_buf), stdin);
            if (status == OK) {
                status = book_search_author(db->bt, auth_buf);
            }
            log_action("With args: %s", auth_buf);
            break;
        }
        case SHOW_BOOKS_STUDENT: {
            printf("Enter isbn: ");
            size_t isbn;
            if (read_size_t(&isbn) != OK) {
                return READ_ERR;
            }
            status = sbook_show_info_isbn(db->sb, isbn);
            log_action("With args: %zu", isbn);
            break;
        }

        case STUDENT_ADD: {
            status = student_add(db->st);
            break;
        }
        case STUDENT_REMOVE: {
            printf("Enter student's record book: ");
            char rb_buf[MAX_RB_LEN + 1];
            status = read_string(rb_buf, sizeof(rb_buf), stdin);
            if (status == OK) {
                if (sbook_student_has_books(db->sb, rb_buf)) {
                    return STUDENT_HAS_BOOKS;
                }
                status = student_delete_rb(db->st, rb_buf);
            }
            log_action("With args: %s", rb_buf);
            break;
        }
        case STUDENT_EDIT: {
            printf("Enter student's record book: ");
            char rb_buf[MAX_RB_LEN + 1];
            status = read_string(rb_buf, sizeof(rb_buf), stdin);
            if (status == OK) {
                status = student_edit_info_rb(db->st, rb_buf);
            }
            log_action("With args: %s", rb_buf);
            break;
        }
        case STUDENT_SHOW_INFO: {
            printf("Enter student's record book: ");
            char rb_buf[MAX_RB_LEN + 1];
            status = read_string(rb_buf, sizeof(rb_buf), stdin);
            if (status == OK) {
                status = student_show_info_rb(db->st, rb_buf);
            }
            log_action("With args: %s", rb_buf);
            break;
        }
        case STUDENT_TABLE_SAVE: {
            status = student_table_save(db->st);
            break;
        }
        case STUDENT_TABLE_RESTORE: {
            printf("Enter filename: ");
            char filename[INIT_SIZE];
            errors_t status = read_string(filename, INIT_SIZE, stdin);
            if (status == OK) {
                status = student_table_restore(&db->st, filename);
            }
            break;
        }
        case STUDENT_SEARCH: {
            printf("Enter student's surname: ");
            char surname_buf[MAX_SURNAME_LEN + 1];
            status = read_string(surname_buf, sizeof(surname_buf), stdin);
            if (status == OK) {
                status = student_search_surname(db->st, surname_buf);
            }
            log_action("With args: %s", surname_buf);
            break;
        }
        case SHOW_STUDENT_BOOKS: {
            printf("Enter student's record book: ");
            char rb_buf[MAX_RB_LEN + 1];
            status = read_string(rb_buf, sizeof(rb_buf), stdin);
            if (status == OK) {
                status = sbook_show_info_rb(db->sb, rb_buf);
            }
            log_action("With args: %s", rb_buf);
            break;
        }
    }
    return status;
}

// Возвращает строковое описание действия
char* str_action(int action) {
    switch (action) {
        case EXIT: {
            return "Exit";
            break;
        }
        case RETURN: {
            return "Return to toplevel menu";
            break;
        }
        case BOOK_ADD: {
            return "Add book";
            break;
        }
        case BOOK_REMOVE: {
            return "Remove book";
            break;
        }
        case BOOK_SHOW_INFO: {
            return "Show book info";
            break;
        }
        case BOOK_PRINT_TABLE: {
            return "Print book table";
            break;
        }
        case BOOK_EDIT: {
            return "Edit book";
            break;
        }
        case BOOK_EDIT_TOTAL: {
            return "Edit book total";
            break;
        }
        case BOOK_GIVE: {
            return "Give student a book";
            break;
        }
        case BOOK_RECEIVE: {
            return "Receive book from a student";
            break;
        }
        case BOOK_TABLE_SAVE: {
            return "Backup book table";
            break;
        }
        case BOOK_TABLE_RESTORE: {
            return "Restore book table";
            break;
        }
        case BOOK_SEARCH: {
            return "Search book by author";
            break;
        }
        case SHOW_BOOKS_STUDENT: {
            return "Show students with given book";
            break;
        }

        case STUDENT_ADD: {
            return "Add student";
            break;
        }
        case STUDENT_REMOVE: {
            return "Remove student";
            break;
        }
        case STUDENT_EDIT: {
            return "Edit student";
            break;
        }
        case STUDENT_SHOW_INFO: {
            return "Show student info";
            break;
        }
        case STUDENT_TABLE_SAVE: {
            return "Backup student table";
            break;
        }
        case STUDENT_TABLE_RESTORE: {
            return "Restore student table";
            break;
        }
        case STUDENT_SEARCH: {
            return "Search student surname";
            break;
        }
        case SHOW_STUDENT_BOOKS: {
            return "Show students with given book";
            break;
        }
    }
    return "None";
}
