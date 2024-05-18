/*
 * Copyright (c) 2024, Constantin Darius-Andrei <darcon2004@gmail.com>
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "utils.h"

typedef struct queue_t {
    unsigned int head;
    unsigned int rear;
    unsigned int queue_size;
    unsigned int current_size;
    size_t data_size;
    void *data;
    void (*free_data)(void *);
} queue_t;

/// @brief A function for initializing a new circular array-based queue.
/// @param size The maximum size of the queue.
/// @param data_size The size of the elements stored in the queue.
/// @param free_data A function used for freeing data when dequeueing.
/// @return A pointer to the newly intialized queue.
queue_t *queue_init(unsigned int size, size_t data_size,
                    void (*free_data)(void *));

/// @brief A function for enqueueing an element in the queue.
/// @param q The queue where the element will be placed.
/// @param data A pointer to the element which will be enqueued. The memory
/// will be copied from the pointer!
void queue_enqueue(queue_t *q, void *data);

/// @brief A function for dequeueing an element from a queue.
/// @param q The queue.
void queue_dequeue(queue_t *q);

/// @brief A function for retrieving the front of a queue.
/// @param q The queue.
/// @return A pointer to the data stored at the front of the queue.
void *queue_front(queue_t *q);

/// @brief A function for checking if a queue is empty.
/// @param q The checked queue.
/// @return 1 if the queue is empty, 0 otherwise.
unsigned char queue_is_empty(queue_t *q);

/// @brief A function for completely freeing a queue.
/// @param q A double pointer to a queue. Upon freeing, the value will be
/// replaced by NULL.
void queue_free(queue_t **q);

#endif /* QUEUE_H */
