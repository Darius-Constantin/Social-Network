/*
 * Copyright (c) 2024, Constantin Darius-Andrei <darcon2004@gmail.com>
 */

#include "queue.h"

queue_t *queue_init(unsigned int queue_size, size_t data_size,
                    void (*free_data)(void *))
{
    DIE(queue_size <= 1, "Queues of length 1 or less are unnecessary!");

    queue_t *q = calloc(1, sizeof(*q));
    DIE(!q, "queue_init queue allocation failed!");

    q->free_data = free_data;
    q->data_size = data_size;
    q->queue_size = queue_size;
    q->current_size = 0;
    q->data = calloc(queue_size, data_size);
    DIE(!q->data, "queue_init data allocation failed!");
    q->rear = 0;
    q->head = 0;

    return q;
}

void queue_enqueue(queue_t *q, void *data)
{
    if (q->current_size == q->queue_size)
        queue_dequeue(q);
    memcpy((char *)q->data + q->data_size * q->rear, data, q->data_size);
    q->rear = (q->rear + 1) % q->queue_size;
    q->current_size++;
}

void queue_dequeue(queue_t *q)
{
    if (q->current_size == 0)
        return;

    if (q->free_data)
        q->free_data(q->data + q->head * q->data_size);
    q->head = (q->head + 1) % q->queue_size;
    q->current_size--;
}

void *queue_front(queue_t *q)
{
    return (char *)q->data + q->head * q->data_size;
}

unsigned char queue_is_empty(queue_t *q)
{
    return !q->current_size;
}

void queue_free(queue_t **q)
{
    while (!queue_is_empty(*q))
        queue_dequeue(*q);
    free((*q)->data);
    free(*q);
    *q = NULL;
}
