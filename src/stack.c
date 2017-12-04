#include "stack.h"

Stack* stack_init(stack_item_free_callback_f stack_item_free_callback) {
    Stack* stack = memory_alloc(sizeof(Stack));
    stack->stack_item_free_callback = stack_item_free_callback;
    stack->head = NULL;

    return stack;
}

void stack_free(Stack** stack) {
    NULL_POINTER_CHECK(stack,);
    NULL_POINTER_CHECK(*stack,);

    StackBaseItem* item = (*stack)->head;
    StackBaseItem* next;
    stack_item_free_callback_f stack_item_free_callback = (*stack)->stack_item_free_callback;

    while(item != NULL) {
        next = item->next;
        if(stack_item_free_callback != NULL)
            stack_item_free_callback(item);
        memory_free(item);
        item = next;
    }
    memory_free(*stack);
    *stack = NULL;
}

void stack_push(Stack* stack, StackBaseItem* item) {
    NULL_POINTER_CHECK(stack,);
    NULL_POINTER_CHECK(item,);
    item->next = stack->head;
    stack->head = item;
}

StackBaseItem* stack_pop(Stack* stack) {
    NULL_POINTER_CHECK(stack, NULL);

    if(stack->head == NULL) return NULL;

    StackBaseItem* item = stack->head;
    stack->head = stack->head->next;
    return item;
}

StackBaseItem* stack_get_by_index(Stack* stack, size_t index) {
    NULL_POINTER_CHECK(stack, NULL);
    StackBaseItem* item = stack->head;
    while(index > 0 && item != NULL) {
        item = item->next;
        index--;
    }
    return item;
}

StackBaseItem* stack_item_int_init(int value)
{
    StackItemInt* item = memory_alloc(sizeof(StackItemInt));
    item->value = value;

    return (StackBaseItem*) item;
}

void stack_pop_free(Stack* stack)
{
    memory_free(stack_pop(stack));
}
