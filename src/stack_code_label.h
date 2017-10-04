#ifndef _STACK_CODE_LABEL_H
#define _STACK_CODE_LABEL_H

#include "stack.h"
#include "debug.h"

typedef struct code_label_t {
    StackBaseItem base;

    const char* label;
} CodeLabel;

void stack_code_label_free(StackBaseItem* item);

Stack stack_code_label_init();

void stack_code_label_push(Stack* stack, const char* label);

CodeLabel* stack_code_label_pop(Stack* stack);

#endif //_STACK_CODE_LABEL_H
