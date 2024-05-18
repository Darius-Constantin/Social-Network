#include "stack.h"

stack_t *stack_init(unsigned int size, size_t data_size, void(* free_data)(void *))
{
    stack_t *new_stack = malloc(sizeof(*new_stack));
    DIE(!new_stack, "new stack allocation failed!");

    new_stack->current_size = 0;
    new_stack->stack_size = size;
    new_stack->data_size = data_size;
    new_stack->data = malloc(data_size * size);
    DIE(!new_stack->data, "new stack data allocation failed!");
    new_stack->top = 0;
    new_stack->free_data = free_data;
}

void stack_push(stack_t *s, void *data)
{
    if (s->current_size == s->stack_size)
        stack_pop(s);

    memcpy((char *)s->data + s->data_size * s->top, data, s->data_size);
    s->current_size++;
}

void stack_pop(stack_t *s)
{
    if (s->current_size == 0)
        return;

    if (s->free_data)
        s->free_data(s->data + s->top * s->data_size);
    s->top--;
    s->current_size--;
}

void *stack_top(stack_t *s)
{
    return (char *)s->data + s->top * s->data_size;
}

unsigned char stack_is_empty(stack_t *s)
{
    return s->current_size == 0 ? 1 : 0;
}

void stack_free(stack_t **s)
{
}
