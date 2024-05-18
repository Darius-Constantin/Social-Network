#ifndef GRAPH_H
#define GRAPH_H

#include "utils.h"
#include "array_hashtable.h"
#include "list.h"

typedef struct graph_t {
    array_hashtable_t *neighbor_table;
    int (*cmp)(void *, void *);
    void *(*add_node)(void *);
    void *(*add_edge)(void *);
    void *(*graph_node_key)(void *);
    void *(*list_node_key)(void *);
    unsigned int no_of_nodes;
} graph_t;

typedef struct graph_node_t {
    void *data;
    list_t *neighbors;
} graph_node_t;

graph_node_t *graph_node_init(graph_t *graph, void *data);

void graph_node_free(graph_node_t **node);

graph_t *graph_init(unsigned int (*node_hash_function)(void *),
                    int (*cmp_function)(void *, void *),
                    void *(*add_node_function)(void *),
                    void *(*add_edge_function)(void *),
                    void *(*graph_node_key)(void *),
                    void *(*list_node_key)(void *),
                    void (*free_data)(void *, void *));

void graph_add_edge(graph_t *graph, void *node1, void *node2);

void graph_remove_edge(graph_t *graph, void *node1, void *node2);

arr_ht_pair_t *graph_add_node(graph_t *graph, void *node);

unsigned int graph_distance_between_nodes(graph_t *graph, void *node1,
                                          void *node2);

vector_t graph_node_grand_neighbors(graph_t *graph, void *node);

vector_t graph_node_common_neighbors(graph_t *graph, void *node1,
                                          void *node2);

graph_node_t *graph_get_node(graph_t *graph, void *node);

void graph_free(graph_t **graph);

#endif /* GRAPH_H */