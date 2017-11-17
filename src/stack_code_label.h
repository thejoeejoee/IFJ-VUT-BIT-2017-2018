#ifndef _STACK_CODE_LABEL_H
#define _STACK_CODE_LABEL_H

#include "stack.h"
#include "debug.h"

typedef struct code_label_t {
    StackBaseItem base;

    char* label;
} CodeLabel;

/**
 * Init stack for code labels.
 * @return initialized stack
 */
Stack* stack_code_label_init();

/**
 * Push table to stack.
 * @param stack instance
 * @param label label to push
 */
void stack_code_label_push(Stack* stack, char* label);

/**
 * Pop code label from stack.
 * @param stack instance
 * @return poped code label
 */
CodeLabel* stack_code_label_pop(Stack* stack);

/**
 * Get code label on head or NULL in case of empty stack.
 * @param stack instance
 * @return ptr to label or NULL
 */
char* stack_code_label_head(Stack* stack);

/**
 * Get label from index - at 0 is head.
 * @param stack instance
 * @param index wanted index
 * @return label
 */
char* stack_code_label_get_by_index(Stack* stack, size_t index);

/**
 * Free one item stack poped from stack.
 * @param label code label to free
 */
void code_label_free(CodeLabel** label);

#endif //_STACK_CODE_LABEL_H
