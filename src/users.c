#include "users.h"

#include <stdlib.h>
#include <string.h>

#include "csvparser.h"

void user_init_array_values(user_table_t *st) {
    for (size_t i = st->cur_size; i < st->max_size; ++i) {
        st->users[i] = NULL;
    }
}

user_table_t *user_init_table(errors_t *status) {
    user_table_t *st = NULL;
    st = malloc(sizeof(user_table_t));
    if (st == NULL) {
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    st->users = malloc(INIT_SIZE * sizeof(user_t *));
    if (st->users == NULL) {
        free(st);
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    st->max_size = INIT_SIZE;
    st->cur_size = 0;

    user_init_array_values(st);

    *status = OK;
    return st;
}

errors_t user_read_row_into_user(const char **row, const int cnt, user_t *user) {
    errors_t status = OK;
    for (int i = 0; i < cnt && status == OK; ++i) {
        switch (i) {
            case U_LOGIN: {
                if (strlen(row[i]) < MAX_LOGIN_LEN) {
                    user->login = malloc((strlen(row[i]) + 1) * sizeof(char));
                    if (user->login) {
                        strcpy(user->login, row[i]);
                    } else {
                        status = MEM_ALLOC_ERR;
                    }
                } else {
                    status = CSV_READ_ERR;
                }
                break;
            }
            case U_PASS: {
                if (strlen(row[i]) < MAX_PASS_LEN) {
                    user->pass = malloc((strlen(row[i]) + 1) * sizeof(char));
                    if (user->pass) {
                        strcpy(user->pass, row[i]);
                    } else {
                        status = MEM_ALLOC_ERR;
                    }
                } else {
                    status = CSV_READ_ERR;
                }
                break;
            }
            case U_BOOK_ACCESS: {
                char *end;
                user->book_access = (bool)strtol(row[i], &end, 10);
                if (*end != 0) {
                    status = CSV_READ_ERR;
                }
                break;
            }
            case U_STUD_ACCESS: {
                char *end;
                user->stud_access = (bool)strtol(row[i], &end, 10);
                if (*end != 0) {
                    status = CSV_READ_ERR;
                }
                break;
            }
        }
    }

    return status;
}

user_t *user_init(errors_t *status) {
    user_t *tmp = malloc(sizeof(user_t));
    if (tmp == NULL) {
        *status = MEM_ALLOC_ERR;
        return NULL;
    }

    tmp->login = NULL;
    tmp->pass = NULL;

    return tmp;
}

errors_t user_extend_table(user_table_t *table, const size_t value) {
    if (value == 0) {
        return OK;
    }

    user_t **tmp = realloc(table->users, (table->max_size + value) * sizeof(user_t *));
    if (tmp) {
        table->users = tmp;
        user_init_array_values(table);
    } else {
        return MEM_ALLOC_ERR;
    }

    table->max_size += value;

    return OK;
}

user_table_t *user_read_csv(const char *file_path, const char *delim, errors_t *status) {
    user_table_t *user_table = user_init_table(status);
    if (user_table == NULL) {
        return NULL;
    }

    CsvParser *csvparser = init_parser(file_path, delim, status);

    if (*status != OK) {
        user_free_table(user_table);
        return NULL;
    }

    CsvRow *row;
    *status = OK;

    user_t *user = NULL;
    while ((row = get_row(csvparser)) && *status == OK) {
        const char **rowFields = get_fields(row);
        const int cnt = get_num_fields(row);
        user = user_init(status);
        if (user) {
            *status = user_read_row_into_user(rowFields, cnt, user);
            if (*status == OK) {
                destroy_row(row);
                if (user_table->cur_size == user_table->max_size) {
                    *status = user_extend_table(user_table, 50);
                }
                if (*status == OK) {
                    user_table->users[user_table->cur_size] = user;
                    ++(user_table->cur_size);
                }
            }
        } else {
            *status = MEM_ALLOC_ERR;
        }
    }

    printf("Users csv load: %s\n", str_err(csvparser->err));
    free_parser(csvparser);

    if (user && *status != OK) {
        user_free(user);
    }

    if ((*status = user_table_compress(&user_table)) != OK) {
        user_free_table(user_table);
    }

    return user_table;
}

errors_t user_table_compress(user_table_t **table) {
    if ((*table)->max_size == (*table)->cur_size) {
        return OK;
    }

    user_t **tmp = realloc((*table)->users, (*table)->cur_size * sizeof(user_t *));
    if (tmp != NULL) {
        (*table)->users = tmp;
        (*table)->max_size = (*table)->cur_size;

        return OK;
    }

    return MEM_ALLOC_ERR;
}

void user_free(user_t *user) {
    if (user == NULL) {
        return;
    }

    if (user->login) {
        free(user->login);
    }

    if (user->pass) {
        free(user->pass);
    }

    free(user);
}

void user_free_table(user_table_t *table) {
    if (table == NULL) {
        return;
    }

    for (size_t i = TABLE_START; i < table->max_size; ++i) {
        if (table->users[i]) {
            user_free(table->users[i]);
        }
    }

    free(table->users);
    free(table);
}

int user_find_login(user_table_t *table, const char *login) {
    for (size_t i = TABLE_START; i < table->cur_size; ++i) {
        if (strcmp(table->users[i]->login, login) == 0) {
            return i;
        }
    }

    return NOT_FOUND;
}

user_t *user_login_into_sysytem(user_table_t *table, const char *login, const char *pass, errors_t *status) {
    int user_index = user_find_login(table, login);
    if (user_index == NOT_FOUND) {
        *status = USER_NOT_FOUND;
        return NULL;
    }

    if (strcmp(table->users[user_index]->pass, pass) == 0) {
        *status = OK;
        return table->users[user_index];
    }

    *status = INCORRECT_PASS;
    return NULL;
}
