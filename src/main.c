#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>

#include "db_executor.h"
#include "io_funcs.h"
#include "logger.h"

FILE* log_file; // глобальная видимость, чтобы из другого файла писать в лог аргументы из функций

int main(void) {
    setbuf(stdout, NULL);

    log_file = fopen("library.log", "a+"); // режим дозаписи в конец
    if (log_file == NULL) {
        raise_error(LOGGER_SETUP_ERR);
        raise_error(FILE_OPEN_ERR);
        return FILE_OPEN_ERR;
    }

    log_set_fp(log_file); // установка файлового дескриптора

    errors_t status;
    printf("Enter login: "); // читаем логин
    char login[INIT_SIZE] = {0};
    status = read_string(login, INIT_SIZE, stdin);
    if (status != OK) {
        printf("Login read error!\n");
        fclose(log_file);
        raise_error(status);
        return status;
    }

    printf("Enter pass: "); // читаем пароль
    char pass[INIT_SIZE] = {0};
    status = read_string(pass, INIT_SIZE, stdin);
    if (status != OK) {
        printf("Pass read error!\n");
        fclose(log_file);
        raise_error(status);
        return status;
    }

    log_action("Attempt to login with: %s:***", login); // логгируем

    db_executor_t* db = db_init(&status); // инициализация модуля, управляющего БД
    if (db == NULL) {
        log_action("Login failed! (db couldn't init)");
        fclose(log_file);
        raise_error(status);
        return status;
    }

    status = db_login(db, login, pass); // логин в систему
    if (status != OK) {
        log_action("Login failed! (%s)", str_err(status));
        raise_error(status);
        db_destroy(db);
        fclose(log_file);
        return status;
    }

    log_action("Login success! (%s:***)", login);

    int action = START_MENU;
    while (action != EXIT) { // пока не действие "выйти"
        db_print_menu(db); // печатаем меню
        status = db_read_action(db, &action); // считываем действие

        if (db->menu == TOP_LEVEL_MENU) { // если пользователь в верхнем меню нужно отправить его в выбранные меню
            if (action != EXIT) {
                if (action >= TOP_LEVEL_MENU && action < NO_MENU) {
                    db->menu = action;
                    db_print_menu(db);
                    status = db_read_action(db, &action);
                } else {
                    status = INCORRECT_CHOICE;
                }
            }
        }

        log_action("%s tried %s (%s)", login, str_action(action), str_err(status)); // логируем попытку действия
        if (status == OK) {
            status = db_execute_action(db, action); // выполняем действие
            log_action("%s executed %s (%s)", login, str_action(action), str_err(status)); // логируем результат выполнения
            raise_error(status);
        } else {
            raise_error(status);
        }
    }

    db_destroy(db); // деинициализируем БД
    fclose(log_file); // закрываем файл

    return OK;
}
