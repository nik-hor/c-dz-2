#ifndef USERS_H
#define USERS_H

#include <stdbool.h>
#include <stddef.h>

#include "errors.h"

#define MAX_LOGIN_LEN 15
#define MAX_PASS_LEN 18

typedef enum {
    U_LOGIN,
    U_PASS,
    U_BOOK_ACCESS,
    U_STUD_ACCESS
} read_user_states_t;

typedef struct {
    char *login;
    char *pass;
    bool book_access;
    bool stud_access;
} user_t;

typedef struct {
    user_t **users;
    size_t cur_size;
    size_t max_size;
} user_table_t;

user_table_t *user_init_table(errors_t *status);
user_t *user_init(errors_t *status);
errors_t user_extend_table(user_table_t *table, const size_t value);

errors_t user_read_row_into_user(const char **row, const int cnt, user_t *user);
user_table_t *user_read_csv(const char *file_path, const char *delim, errors_t *status);

errors_t user_table_compress(user_table_t **table);

void user_free(user_t *user);
void user_free_table(user_table_t *table);

int user_find_login(user_table_t *table, const char *login);
user_t *user_login_into_sysytem(user_table_t *table, const char *login, const char *pass, errors_t *status);

#endif
