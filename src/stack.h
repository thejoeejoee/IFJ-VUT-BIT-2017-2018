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

Stack* stack_init(stack_item_free_callback_f stack_item_free_callback);

void stack_free(Stack** stack);

void stack_push(Stack* stack, StackBaseItem* item);

StackBaseItem* stack_pop(Stack* stack);

StackBaseItem* stack_get_by_index(Stack* stack, size_t index);

#endif //_STACK_H
