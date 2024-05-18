#include "graph.h"
#include "queue.h"

graph_node_t *graph_node_init(graph_t *graph, void *data)
{
    graph_node_t *new_node = malloc(sizeof(*new_node));
    DIE(!new_node, "new node allocation failed!");

    new_node->data = graph->add_node(data);
    new_node->neighbors = list_init(graph->cmp, graph->add_edge,
                                    graph->list_node_key, NULL);

    return new_node;
}

graph_t *graph_init(unsigned int (*node_hash_function)(void *),
                    int (*cmp_function)(void *, void *),
                    void *(*add_node_function)(void *),
                    void *(*add_edge_function)(void *),
                    void *(*graph_node_key)(void *),
                    void *(*list_node_key)(void *),
                    void (*free_data)(void *, void *))
{
    graph_t *new_graph = malloc(sizeof(*new_graph));
    new_graph->neighbor_table = arr_ht_init(INIT_MAX_BUCKETS,
                                            node_hash_function,
                                            cmp_function,
                                            free_data);
    new_graph->cmp = cmp_function;
    new_graph->add_node = add_node_function;
    new_graph->add_edge = add_edge_function;
    new_graph->graph_node_key = graph_node_key;
    new_graph->list_node_key = list_node_key;
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

    list_remove_data(((graph_node_t *)pair1->value)->neighbors, pair2);
    list_remove_data(((graph_node_t *)pair2->value)->neighbors, pair1);
}

arr_ht_pair_t *graph_add_node(graph_t *graph, void *node)
{
    graph_node_t *added_node = graph_node_init(graph, node);
    arr_ht_pair_t *added_pair = arr_ht_put(graph->neighbor_table,
                                           graph->graph_node_key(added_node->data),
                                           added_node);
    graph->no_of_nodes = graph->neighbor_table->current_size;
    return added_pair;
}

unsigned int graph_distance_between_nodes(graph_t *graph, void *node1,
                                          void *node2)
{
    arr_ht_pair_t **buckets = graph->neighbor_table->buckets;
    unsigned int distance = INFTY;

    long long just_visited_idx = arr_ht_has_key(graph->neighbor_table, node1);
    long long target_idx = arr_ht_has_key(graph->neighbor_table, node2);
    if (just_visited_idx == -1 || target_idx == -1)
        return INFTY;
    if (target_idx == just_visited_idx)
        return 0;

    unsigned int *visited = calloc(graph->neighbor_table->number_of_buckets,
                                   sizeof(*visited));
    DIE(!visited, "visited allocation failed!");

    visited[just_visited_idx] = 1;
    queue_t *list_queue = queue_init(graph->no_of_nodes, sizeof(list_t *),
                                     NULL);
    queue_t *idx_queue = queue_init(graph->no_of_nodes, sizeof(unsigned int),
                                    NULL);
    queue_enqueue(list_queue, &((graph_node_t *)buckets[just_visited_idx]->value)->neighbors);
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
                int64_t neighbor_idx = arr_ht_has_key(graph->neighbor_table,
                                                       current_key);
                if (neighbor_idx != -1 && visited[neighbor_idx] == 0) {
                    visited[neighbor_idx] = visited[just_visited_idx] + 1;
                    queue_enqueue(list_queue, &((graph_node_t *)buckets[neighbor_idx]->value)->neighbors);
                    queue_enqueue(idx_queue, &neighbor_idx);
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

vector_t graph_node_grand_neighbors(graph_t *graph, void *node)
{
    long long just_visited_idx = arr_ht_has_key(graph->neighbor_table, node);
    if (just_visited_idx == -1)
        return (vector_t){0, NULL};

    arr_ht_pair_t **buckets = graph->neighbor_table->buckets;
    unsigned int *visited = calloc(graph->neighbor_table->number_of_buckets,
                                   sizeof(*visited));
    DIE(!visited, "visited allocation failed!");

    visited[just_visited_idx] = 1;
    queue_t *list_queue = queue_init(graph->no_of_nodes, sizeof(list_t *),
                                     NULL);
    queue_t *idx_queue = queue_init(graph->no_of_nodes, sizeof(unsigned int),
                                    NULL);

    list_t *current_list = ((graph_node_t *)buckets[just_visited_idx]->value)->neighbors;
    ll_node_t *curr = current_list->head;
    if (curr) {
        do {
            void *current_key = ((arr_ht_pair_t *)curr->data)->key;
            int64_t neighbor_idx = arr_ht_has_key(graph->neighbor_table,
                                                  current_key);
            if (neighbor_idx != -1 && visited[neighbor_idx] == 0) {

                visited[neighbor_idx]++;
                queue_enqueue(list_queue, &((graph_node_t *)buckets[neighbor_idx]->value)->neighbors);
                queue_enqueue(idx_queue, &neighbor_idx);
            }
            curr = curr->next;
        } while (curr != current_list->head);
    }

    graph_node_t **result = malloc(graph->no_of_nodes * sizeof(*result));
    unsigned int current_idx = 0;
    DIE(!result, "result allocation failed!");

    while (!queue_is_empty(list_queue)) {
        list_t *current_list = *(list_t **)queue_front(list_queue);
        just_visited_idx = *(unsigned int *)queue_front(idx_queue);

        ll_node_t *curr = current_list->head;
        if (curr) {
            do {
                void *current_key = ((arr_ht_pair_t *)curr->data)->key;
                int64_t neighbor_idx = arr_ht_has_key(graph->neighbor_table,
                                                         current_key);
                if (neighbor_idx != -1 && visited[neighbor_idx] == 0) {
                    visited[neighbor_idx]++;
                    result[current_idx++] = buckets[neighbor_idx]->value;
                }
                curr = curr->next;
            } while (curr != current_list->head);
        }

        queue_dequeue(list_queue);
        queue_dequeue(idx_queue);
    }

    free(visited);
    queue_free(&list_queue);
    queue_free(&idx_queue);

    if (current_idx) {
        graph_node_t **tmp = realloc(result, current_idx * sizeof(*result));
        DIE(!tmp, "result realloc failed!");
        result = tmp;
    } else {
        free(result);
        result = NULL;
    }

    return (vector_t){current_idx, result};
}

vector_t graph_node_common_neighbors(graph_t *graph, void *node1, void *node2)
{
    long long node1_idx = arr_ht_has_key(graph->neighbor_table, node1);
    long long node2_idx = arr_ht_has_key(graph->neighbor_table, node2);
    if (node1_idx == -1 || node2_idx == -1)
        return (vector_t){0, NULL};

    arr_ht_pair_t **buckets = graph->neighbor_table->buckets;
    unsigned int *visited = calloc(graph->neighbor_table->number_of_buckets,
                                   sizeof(*visited));
    DIE(!visited, "visited allocation failed!");

    ll_node_t *curr;

    curr = ((graph_node_t *)buckets[node1_idx]->value)->neighbors->head;
    if (curr) {
        do {
            void *current_key = ((arr_ht_pair_t *)curr->data)->key;
            int64_t neighbor_idx = arr_ht_has_key(graph->neighbor_table,
                                                  current_key);
            visited[neighbor_idx]++;
            curr = curr->next;
        } while (curr != ((graph_node_t *)buckets[node1_idx]->value)->neighbors->head);
    }

    graph_node_t **result = malloc(graph->no_of_nodes * sizeof(*result));
    unsigned int current_idx = 0;
    DIE(!result, "result allocation failed!");

    curr = ((graph_node_t *)buckets[node2_idx]->value)->neighbors->head;
    if (curr) {
        do {
            void *current_key = ((arr_ht_pair_t *)curr->data)->key;
            int64_t neighbor_idx = arr_ht_has_key(graph->neighbor_table,
                                                  current_key);
            if (visited[neighbor_idx])
                result[current_idx++] = buckets[neighbor_idx]->value;
            curr = curr->next;
        } while (curr != ((graph_node_t *)buckets[node2_idx]->value)->neighbors->head);
    }

    free(visited);

    if (current_idx) {
        graph_node_t **tmp = realloc(result, current_idx * sizeof(*result));
        DIE(!tmp, "result realloc failed!");
        result = tmp;
    } else {
        free(result);
        result = NULL;
    }

    return (vector_t){current_idx, result};
}

graph_node_t *graph_get_node(graph_t *graph, void *node)
{
    arr_ht_pair_t *pair = arr_ht_retrieve_value(graph->neighbor_table, node);
    return pair ? pair->value : NULL;
}

void graph_free(graph_t **graph)
{
    arr_ht_free(&(*graph)->neighbor_table);
    free(*graph);
    *graph = NULL;
}