#include "io_funcs.h"

#include <string.h>

// функция чтения строки с потока ввода
errors_t read_string(char* dst, size_t size, FILE* stream) {
    if (fgets(dst, size, stream) != NULL) {
        int len = strlen(dst);
        if (dst[len - 1] == '\n') {
            dst[len - 1] = 0;
            return OK;
        } else {
            return STRING_OVERFLOW_ERR;
        }
    } else {
        return STRING_READ_ERR;
    }
}

errors_t read_size_t(size_t* dst) {
    errors_t status = OK;
    if (scanf("%zu", dst) != 1) {
        status = READ_ERR;
    }

    char buf[10];
    read_string(buf, sizeof(buf), stdin);

    return status;
}

errors_t read_int(int* dst) {
    errors_t status = OK;
    if (scanf("%d", dst) != 1) {
        status = READ_ERR;
    }

    char buf[10];
    read_string(buf, sizeof(buf), stdin);

    return status;
}

// Разедлы с меню
void top_level_menu() {
    puts("-1. Exit");
    puts(" 1. Book menu");
    puts(" 2. Student menu");
}

void book_menu(bool full_access) {
    puts("-1. Exit");
    if (full_access) {
        puts(" 0. Return");
    }

    puts(" 1.  Add book");
    puts(" 2.  Remove book");
    puts(" 3.  Show book info");
    puts(" 4.  Print book table");
    puts(" 5.  Edit book");
    puts(" 6.  Edit book total");
    puts(" 7.  Give book to a student");
    puts(" 8.  Receive book from student");
    puts(" 9.  Backup book table");
    puts(" 10. Restore book table");
    puts(" 11. Search book by atuhor");
    puts(" 12. Find students with given book");
}

void student_menu(bool full_access) {
    puts("-1. Exit");
    if (full_access) {
        puts(" 0. Return");
    }

    puts(" 13. Add student");
    puts(" 14. Remove student");
    puts(" 15. Edit student");
    puts(" 16. Show student info");
    puts(" 17. Backup students table");
    puts(" 18. Restore student table");
    puts(" 19. Search student by surname");
    puts(" 20. Show student books");
}