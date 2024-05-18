/*
 * Copyright (c) 2024, Constantin Darius-Andrei <darcon2004@gmail.com>
 */

#include "list.h"

list_t *list_init(int (*cmp_function)(void *, void *),
				  void *(*cpy_function)(void *),
				  void *(*access_node_key)(void *),
				  void (*free_data)(void *))
{
	list_t *new_list = malloc(sizeof(*new_list));
	new_list->cmp = cmp_function;
	new_list->cpy = cpy_function;
	new_list->access_node_key = access_node_key;
	new_list->free_data = free_data;
	new_list->head = NULL;
	new_list->tail = NULL;
	new_list->length = 0;
	return new_list;
}

void list_append_data(list_t *list, void *data)
{
	list_append_node(list, node_init(list->cpy(data)));
}

void list_append_node(list_t *list, ll_node_t *target)
{
	if (!list->tail) {
		list->head = target;
		list->tail = target;
		list->length++;
		return;
	}

	ll_node_t *curr = list->head;
	do {
		if (list->cmp(list->access_node_key(target->data),
					  list->access_node_key(curr->data)) == 0) {
			list_free_node(list, target);
			return;
		}
		curr = curr->next;
	} while (curr->next != list->head);

	list->length++;
	target->next = list->head;
	curr->next = target;
	list->tail = target;
}

void list_remove_data(list_t *list, void *data)
{
	ll_node_t *curr = list->head;
	if (!curr)
		return;

	if (list->cmp(curr->data, data) == 0) {
		list->length--;
		list->head = curr->next;
		list->tail->next = list->head;
		if (list->head == curr) {
			list->tail = NULL;
			list->head = NULL;
		}
		list_free_node(list, curr);
		return;
	}

	do {
		if (list->cmp(curr->next->data, data) == 0) {
			list->length--;
			ll_node_t *tmp = curr->next;
			if (tmp == list->tail)
				list->tail = curr;
			curr->next = tmp->next;
			list_free_node(list, tmp);
			return;
		}
		curr = curr->next;
	} while (curr->next != list->head);
}

void list_free(list_t **list)
{
	if (!*list)
		return;

	ll_node_t *curr = (*list)->head;
	if (curr) {
		do {
			ll_node_t *next = curr->next;
			if ((*list)->free_data)
				(*list)->free_data(curr->data);
			free(curr);
			curr = next;
		} while (curr != (*list)->head);
	}

	free(*list);
	*list = NULL;
}

void list_free_node(list_t *list, ll_node_t *node)
{
	if (list->free_data)
		list->free_data(node->data);
	free(node);
}
