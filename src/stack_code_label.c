#include "stack_code_label.h"

Stack* stack_code_label_init() {
    return stack_init(NULL);
}

void stack_code_label_push(Stack* stack, char* label) {
    NULL_POINTER_CHECK(stack,);
    NULL_POINTER_CHECK(label,);

    CodeLabel* code_label = (CodeLabel*) memory_alloc(sizeof(CodeLabel));
    code_label->base.next = NULL;
    code_label->label = label;

    stack_push(stack, (StackBaseItem*) code_label);
}

CodeLabel* stack_code_label_pop(Stack* stack) {
    NULL_POINTER_CHECK(stack, NULL);

    CodeLabel* code_label = (CodeLabel*) stack_pop(stack);

    return code_label;
}

char* stack_code_label_head(Stack* stack) {
    NULL_POINTER_CHECK(stack, NULL);
    NULL_POINTER_CHECK(stack->head, NULL);

    return ((CodeLabel*) stack->head)->label;
}
