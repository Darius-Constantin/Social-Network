#include "node.h"

ll_node_t *node_init(void *data)
{
    ll_node_t *new_node = malloc(sizeof(*new_node));
    DIE(!new_node, "new_node allocation");

    new_node->next = new_node;
    new_node->data = data;

    return new_node;
}