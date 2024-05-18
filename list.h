/*
 * Copyright (c) 2024, Constantin Darius-Andrei <darcon2004@gmail.com>
 */

#ifndef LIST_H
#define LIST_H

#include "utils.h"
#include "node.h"

typedef struct list_t {
    ll_node_t *head;
    ll_node_t *tail;
    unsigned int length;
    int (*cmp)(void *, void *);
    void *(*cpy)(void *);
    void *(*access_node_key)(void *);
    void (*free_data)(void *);
} list_t;


list_t *list_init(int (*cmp_function)(void *, void *),
                  void *(*cpy_function)(void *),
                  void *(*access_node_key)(void *),
                  void (*free_data)(void *));

void list_append_data(list_t *list, void *data);

void list_append_node(list_t *list, ll_node_t *target);

void list_remove_data(list_t *list, void *data);

void list_free(list_t **list);

void list_free_node(list_t *list, ll_node_t *node);

#endif /* LIST_H */
