#include "graph.h"

graph_node_t *graph_node_init(graph_t *graph, void *data)
{
    graph_node_t *new_node = malloc(sizeof(*new_node));
    DIE(!new_node, "new node allocation failed!");

    new_node->data = graph->add_node(data);
    new_node->neighbors = list_init(graph->cmp, graph->add_edge,
                                    graph->free_data);

    return new_node;
}

graph_t *graph_init(unsigned int (*node_hash_function)(void *),
                    int (*cmp_function)(void *, void *),
                    void *(*add_node_function)(void *),
                    void *(*add_edge_function)(void *),
                    void (*free_data)(void *))
{
    graph_t *new_graph = malloc(sizeof(*new_graph));
    new_graph->neighbor_table = arr_ht_init(INIT_MAX_BUCKETS,
                                            node_hash_function,
                                            cmp_function);
    new_graph->cmp = cmp_function;
    new_graph->add_node = add_node_function;
    new_graph->add_edge = add_edge_function;
    new_graph->free_data = free_data;
    new_graph->no_of_nodes = 0;

    return new_graph;
}

void graph_add_edge(graph_t *graph, void *node1, void *node2)
{
    arr_ht_pair_t *pair1 = arr_ht_retrieve_value(graph->neighbor_table, node1);
    if (!pair1)
        return;

    arr_ht_pair_t *pair2 = arr_ht_retrieve_value(graph->neighbor_table, node2);
    if (!pair2)
        return;

    list_append_data(((graph_node_t *)pair1->value)->neighbors, pair2);
    list_append_data(((graph_node_t *)pair2->value)->neighbors, pair1);
}

void graph_remove_edge(graph_t *graph, void *node1, void *node2)
{
    arr_ht_pair_t *pair1 = arr_ht_retrieve_value(graph->neighbor_table, node1);
    if (!pair1)
        return;

    arr_ht_pair_t *pair2 = arr_ht_retrieve_value(graph->neighbor_table, node2);
    if (!pair2)
        return;

    list_remove_data(pair1->value, pair2);
    list_remove_data(pair2->value, pair1);
}

arr_ht_pair_t *graph_add_node(graph_t *graph, void *node)
{
    graph_node_t *added_node = graph_node_init(graph, node);
    arr_ht_pair_t *added_pair = arr_ht_put(graph->neighbor_table,
                                           added_node->data,
                                           added_node);
    graph->no_of_nodes = graph->neighbor_table->current_size;
    return added_pair;
}

unsigned int graph_distance_between_nodes(graph_t *graph, void *node1,
                                          void *node2)
{
    arr_ht_pair_t **buckets = graph->neighbor_table->buckets;
    unsigned int distance = INFTY;
    unsigned int *visited = malloc(graph->no_of_nodes * sizeof(*visited));
    DIE(!visited, "visited allocation failed!");

    long long just_visited_idx = arr_ht_has_key(graph->neighbor_table, node1);
    long long target_index = arr_ht_has_key(graph->neighbor_table, node2);
    if (just_visited_idx == -1 || target_index == -1)
        return INFTY;
    if (target_index == just_visited_idx)
        return 0;

    visited[just_visited_idx] = 1;
    queue_t *list_queue = queue_init(graph->no_of_nodes, sizeof(list_t *),
                                     free);
    queue_t *idx_queue = queue_init(graph->no_of_nodes, sizeof(unsigned int),
                                    NULL);
    queue_enqueue(list_queue, &buckets[just_visited_idx]->value);
    queue_enqueue(idx_queue, &just_visited_idx);
    while (!queue_is_empty(list_queue)) {
        list_t *current_list = *(list_t **)queue_front(list_queue);
        just_visited_idx = *(unsigned int *)queue_front(idx_queue);

        ll_node_t *curr = current_list->head;
        if (curr) {
            do {
                void *current_key = ((arr_ht_pair_t *)curr->data)->key;
                if (graph->cmp(current_key, node2) == 0) {
                    distance = visited[just_visited_idx];
                    goto FOUND_TARGET;
                }
                uint32_t to_be_queued = arr_ht_has_key(graph->neighbor_table,
                                                       current_key);
                if (visited[to_be_queued] == 0) {
                    visited[to_be_queued] = visited[just_visited_idx] + 1;
                    queue_enqueue(list_queue, &buckets[to_be_queued]->value);
                    queue_enqueue(idx_queue, &to_be_queued);
                }
                curr = curr->next;
            } while (curr != current_list->head);
        }

        queue_dequeue(list_queue);
        queue_dequeue(idx_queue);
    }

FOUND_TARGET:
    free(visited);
    queue_free(&list_queue);
    queue_free(&idx_queue);
    return distance;
}

unsigned int *graph_node_grand_neighbors(graph_t *graph, void *node)
{
    arr_ht_pair_t **buckets = graph->neighbor_table->buckets;
    unsigned int *visited = malloc(graph->no_of_nodes * sizeof(*visited));
    DIE(!visited, "visited allocation failed!");

    long long just_visited_idx = arr_ht_has_key(graph->neighbor_table, node);
    if (just_visited_idx == -1)
        return NULL;

    visited[just_visited_idx] = 1;
    queue_t *list_queue = queue_init(graph->no_of_nodes, sizeof(list_t *),
                                     free);
    queue_t *idx_queue = queue_init(graph->no_of_nodes, sizeof(unsigned int),
                                    NULL);
    queue_enqueue(list_queue, &buckets[just_visited_idx]->value);

    list_t *current_list = buckets[just_visited_idx]->value;

    ll_node_t *curr = current_list->head;
    if (curr) {
        do {
            void *current_key = ((arr_ht_pair_t *)curr->data)->key;
            uint32_t to_be_queued = arr_ht_has_key(graph->neighbor_table,
                                                   current_key);
            if (visited[to_be_queued] == 0) {
                visited[to_be_queued]++;
                queue_enqueue(list_queue, &buckets[to_be_queued]->value);
                queue_enqueue(idx_queue, &to_be_queued);
            }
            curr = curr->next;
        } while (curr != current_list->head);
    }

    unsigned int *result = malloc(graph->no_of_nodes * sizeof(unsigned int));
    unsigned int current_idx = 0;
    DIE(!result, "result allocation failed!");

    while (!queue_is_empty(list_queue)) {
        list_t *current_list = *(list_t **)queue_front(list_queue);
        just_visited_idx = *(unsigned int *)queue_front(idx_queue);

        ll_node_t *curr = current_list->head;
        if (curr) {
            do {
                void *current_key = ((arr_ht_pair_t *)curr->data)->key;
                uint32_t grand_neighbor = arr_ht_has_key(graph->neighbor_table,
                                                         current_key);
                if (visited[grand_neighbor] == 0)
                    result[current_idx++] = grand_neighbor;
                curr = curr->next;
            } while (curr != current_list->head);
        }

        queue_dequeue(list_queue);
        queue_dequeue(idx_queue);
    }

    free(visited);
    queue_free(&list_queue);
    queue_free(&idx_queue);

    unsigned int *tmp = realloc(result, current_idx * sizeof(unsigned int));
    DIE(!tmp, "result realloc failed!");
    result = tmp;

    return result;
}

// Daca sunt cele doua noduri neighbors, sa nu uit sa le scot! (sau s-au scos singure?)
unsigned int *graph_node_common_neighbors(graph_t *graph, void *node1, void *node2)
{
    arr_ht_pair_t **buckets = graph->neighbor_table->buckets;
    unsigned int *visited = malloc(graph->no_of_nodes * sizeof(*visited));
    DIE(!visited, "visited allocation failed!");

    long long node1_idx = arr_ht_has_key(graph->neighbor_table, node1);
    long long node2_idx = arr_ht_has_key(graph->neighbor_table, node2);
    if (node1_idx == -1 || node2_idx == -1)
        return NULL;

    ll_node_t *curr;

    curr = ((list_t *)buckets[node1_idx]->value)->head;
    if (curr) {
        do {
            void *current_key = ((arr_ht_pair_t *)curr->data)->key;
            uint8_t neighbor_idx = arr_ht_has_key(graph->neighbor_table,
                                                  current_key);
            visited[neighbor_idx]++;
            curr = curr->next;
        } while (curr != ((list_t *)buckets[node1_idx]->value)->head);
    }

    unsigned int *result = malloc(graph->no_of_nodes * sizeof(unsigned int));
    unsigned int current_idx = 0;
    DIE(!result, "result allocation failed!");

    curr = ((list_t *)buckets[node2_idx]->value)->head;
    if (curr) {
        do {
            void *current_key = ((arr_ht_pair_t *)curr->data)->key;
            uint8_t neighbor_idx = arr_ht_has_key(graph->neighbor_table,
                                                  current_key);
            if (visited[neighbor_idx])
                result[current_idx++] = neighbor_idx;
            curr = curr->next;
        } while (curr != ((list_t *)buckets[node2_idx]->value)->head);
    }

    unsigned int *tmp = realloc(result, current_idx * sizeof(unsigned int));
    DIE(!tmp, "result realloc failed!");
    result = tmp;

    free(visited);
    return result;
}

graph_node_t *graph_get_node(graph_t *graph, void *node)
{
    arr_ht_pair_t *pair = arr_ht_retrieve_value(graph->neighbor_table, node);
    return pair ? pair->value : NULL;
}
