#ifndef STACK_H
#define STACK_H

#include "utils.h"

typedef struct stack_t {
    unsigned int top;
    unsigned int current_size;
    unsigned int stack_size;
    size_t data_size;
    void *data;
    void (*free_data)(void *);
} stack_t;

/// @brief A function for initializing a new stack.
/// @param size The maximum size of the stack.
/// @param data_size The size of the elements stored in the stack.
/// @param free_data A function used for freeing data when popping.
/// @return A pointer to the newly intialized stack.
stack_t *stack_init(unsigned int size, size_t data_size,
                    void (*free_data)(void *));

/// @brief A function for pushing an element in the stack.
/// @param s The stack where the element will be placed.
/// @param data A pointer to the element which will be pushed. The memory
/// will be copied from the pointer!
void stack_push(stack_t *s, void *data);

/// @brief A function for popping an element from a stack.
/// @param s The stack.
void stack_pop(stack_t *s);

/// @brief A function for retrieving the top of a stack.
/// @param s The stack.
/// @return A pointer to the data stored at the top of the stack.
void *stack_top(stack_t *s);

/// @brief A function for checking if a stack is empty.
/// @param s The checked stack.
/// @return 1 if the stack is empty, 0 otherwise.
unsigned char stack_is_empty(stack_t *s);

/// @brief A function for completely freeing a stack.
/// @param s A double pointer to a stack. Upon freeing, the value will be
/// replaced by NULL.
void stack_free(stack_t **s);

#endif /* STACK_H */
