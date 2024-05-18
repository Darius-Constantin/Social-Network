#ifndef SOCIAL_MANAGER_H
#define SOCIAL_MANAGER_H

#include "utils.h"
#include "array_hashtable.h"
#include "graph.h"

typedef struct social_manager_t {
    unsigned int post_id;
    graph_t *user_graph;
    array_hashtable_t *post_table;
} social_manager_t;

#include "users.h"
#include "posts.h"

social_manager_t social_manager_init();

void social_manager_free_contents(social_manager_t social_manager);

#endif /* SOCIAL_MANAGER_H */