#include "stack_code_label.h"

Stack stack_code_label_init() {
    return stack_init(stack_code_label_free);
}

void stack_code_label_free(StackBaseItem* item) {
    CodeLabel* code_label = (CodeLabel*) item;

    memory_free(code_label->label);
    memory_free(code_label);
}

void stack_code_label_push(Stack* stack, const char* label) {
    NULL_POINTER_CHECK(stack,);
    NULL_POINTER_CHECK(label,);

    CodeLabel* code_label = (CodeLabel*) (memory_alloc(sizeof(CodeLabel)));
    code_label->base = NULL;
    code_label->label = label;

    stack_push(stack, (StackBaseItem*) code_label);
}

CodeLabel* stack_code_label_pop(Stack* stack) {
    NULL_POINTER_CHECK(stack, NULL);

    CodeLabel* code_label = (CodeLabel*) (stack_pop(stack));

    return code_label;
}
