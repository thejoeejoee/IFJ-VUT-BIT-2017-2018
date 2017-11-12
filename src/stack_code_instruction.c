#include "stack_code_instruction.h"


Stack* stack_code_instruction_init() {
    return stack_init(NULL);
}

void stack_code_instruction_push(Stack* stack, CodeInstruction* instruction) {
    NULL_POINTER_CHECK(stack,);
    NULL_POINTER_CHECK(instruction,);

    StackCodeInstructionItem* stack_item = (StackCodeInstructionItem*) memory_alloc(sizeof(StackCodeInstructionItem));
    stack_item->base.next = NULL;
    stack_item->instruction = instruction;
    stack_push(stack, (StackBaseItem*) stack_item);
}

CodeInstruction* stack_code_instruction_pop(Stack* stack) {
    NULL_POINTER_CHECK(stack, NULL);

    StackCodeInstructionItem* item = (StackCodeInstructionItem*) stack_pop(stack);
    if(item == NULL) return NULL;
    CodeInstruction* instruction = item->instruction;
    memory_free(item);
    return instruction;
}

CodeInstruction* stack_code_instruction_head(Stack* stack) {
    NULL_POINTER_CHECK(stack, NULL);
    if(stack->head == NULL) return NULL; // without warning

    return ((StackCodeInstructionItem*) stack->head)->instruction;
}

