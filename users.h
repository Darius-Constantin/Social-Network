#ifndef USERS_H
#define USERS_H

#include <stdint.h>
#include "utils.h"
#include "posts.h"
#include "list.h"

typedef struct user_t {
    unsigned int user_id;
    char *username;
    list_t *posts;
} user_t;

user_t *user_init(char *username);

void *create_user_from_username(void *username);

/**
 * Initializes the user list
*/
void init_users(void);

/**
 * Find the user_id of a user by it's name
 *
 * @param name - The name of the user
 * @return the id of the user, of -1 if name is not found
*/
uint16_t get_user_id(char *name);

/**
 * Find the user_id of a user by it's name
 *
 * @param id - The id of a user
 * @return the name of a user, of NULL if not found
*/
char *get_user_name(uint16_t id);

unsigned int hash_username(void *name);

void *cpy_username(void *name);

int cmp_users(void *user1, void *user2);

#endif /* USERS_H */