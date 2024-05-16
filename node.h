#ifndef NODE_H
#define NODE_H

#include "utils.h"

typedef struct linked_node_t {
    void *data;
    struct linked_node_t *next;
} ll_node_t;

/// @brief A function for initializing a new circular doubly linked node.
/// @param data The pointer which will be stored in the node. Does not copy
/// memory! The sameself pointer will be stored.
/// @return The newly created node.
ll_node_t *node_init(void *data);

#endif /* NODE_H */
