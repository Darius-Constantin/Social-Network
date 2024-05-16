#ifndef POSTS_H
#define POSTS_H

#include "utils.h"
#include "social_manager.h"
#include "array_hashtable.h"
#include "list.h"
#include "users.h"

typedef struct post_t {
    unsigned int id;
    unsigned int user_id;
    char *title;
    struct post_t **events;
} post_t;

/**
 * Function that handles the calling of every command from task 2
 *
 * Please add any necessary parameters to the functions
*/

void handle_input_posts(char *input);

void post_create(social_manager_t *social_manager, char *username, char *title);

int cmp_posts(void *post1, void *post2);

#endif // POSTS_H
