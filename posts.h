#ifndef POSTS_H
#define POSTS_H

#include "utils.h"
#include "list.h"
#include "queue.h"
#include "stack.h"

typedef struct post_t {
    unsigned int id;
    unsigned int user_id;
    char *title;
    struct post_t *parent_post;
    list_t *events;
    list_t *likes;
} post_t;

#include "social_manager.h"

void handle_input_posts(char *input, social_manager_t *social_manager);

post_t *post_init(social_manager_t *social_manager, char *username, char *title);

post_t *repost_init(social_manager_t *social_manager, char *username,
                    unsigned int post_id, unsigned int repost_id);

post_t *repost_find_by_id(social_manager_t *social_manager, post_t *root,
                          unsigned int target);

post_t *post_find_by_id(social_manager_t *social_manager,
                        unsigned int post_id, unsigned int repost_id);

vector_t post_find_all_reposts(social_manager_t *social_manager,
                               unsigned int post_id, unsigned int repost_id);

void post_find_all_reposts_recursive(social_manager_t *social_manager,
                                     post_t *original_post,
                                     vector_t *accumulator);

void post_delete(social_manager_t *social_manager, post_t *post);

void post_free(void **post);

int cmp_posts(void *post1, void *post2);

void *get_post_id_from_post(void *post);

uint8_t post_like(social_manager_t *social_manager, char *username,
				  post_t *post);

post_t *post_with_greatest_value(post_t *root,
                                 void *(*access_value)(void *),
                                 int (*cmp)(void *, void *));

#endif /* POSTS_H */
