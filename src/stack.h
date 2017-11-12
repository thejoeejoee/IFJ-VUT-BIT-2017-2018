#ifndef _STACK_H
#define _STACK_H

#include <stdlib.h>
#include "debug.h"
#include "memory.h"

typedef struct stack_base_item_t {
    struct stack_base_item_t* next;
} StackBaseItem;

typedef void (* stack_item_free_callback_f)(StackBaseItem* item);

typedef struct stack__t {
    stack_item_free_callback_f stack_item_free_callback;
    StackBaseItem* head;
} Stack;

/**
 * @brief Constructor for stack
 *
 * @param stack_item_free_callback
 * @return
 */
Stack* stack_init(stack_item_free_callback_f stack_item_free_callback);

void stack_free(Stack** stack);

/**
 * @brief Push pointer into stack
 *
 * @param Stack* stack Pointer to stack
 * @param StackBaseItem* item Item which will be pushed
 */
void stack_push(Stack* stack, StackBaseItem* item);

/**
 * @brief Pop item from stack and return pointer on value
 *
 * @param Stack* stack Pointer to stack
 * @return StackBaseItem* Popped item
 */
StackBaseItem* stack_pop(Stack* stack);

StackBaseItem* stack_get_by_index(Stack* stack, size_t index);

#endif //_STACK_H
