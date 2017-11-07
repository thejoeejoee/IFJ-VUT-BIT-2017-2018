#ifndef _STACK_CODE_INSTRUCTION_H
#define _STACK_CODE_INSTRUCTION_H

#include "stack.h"
#include "debug.h"
#include "code_instruction.h"

typedef struct stack_code_instruction_item_t {
    StackBaseItem base;

    CodeInstruction* instruction;
} StackCodeInstructionItem;

Stack* stack_code_instruction_init();

void stack_code_instruction_push(Stack* stack, CodeInstruction* instruction);

CodeInstruction* stack_code_instruction_pop(Stack* stack);

CodeInstruction* stack_code_instruction_head(Stack* stack);

#endif //_STACK_CODE_INSTRUCTION_H
