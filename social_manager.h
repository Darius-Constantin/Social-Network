#ifndef SOCIAL_MANAGER_H
#define SOCIAL_MANAGER_H

#include "utils.h"
#include "graph.h"

typedef struct social_manager_t {
    unsigned int post_id;
    graph_t *user_graph;
    array_hashtable_t *post_table;
} social_manager_t;

#endif /* SOCIAL_MANAGER_H */