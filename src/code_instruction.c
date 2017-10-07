#include "code_instruction.h"
#include "memory.h"

CodeInstruction* code_instruction_init(
        TypeInstruction type,
        CodeInstructionOperand* op0,
        CodeInstructionOperand* op1,
        CodeInstructionOperand* op2
) {
    CodeInstruction* instruction = memory_alloc(sizeof(CodeInstruction));
    instruction->type = type;
    instruction->op0 = op0;
    instruction->op1 = op1;
    instruction->op2 = op2;

    instruction->prev = instruction->next = NULL;

    return instruction;
}

void code_instruction_free(CodeInstruction** instruction) {
    NULL_POINTER_CHECK(instruction,);
    NULL_POINTER_CHECK(*instruction,);

    if((*instruction)->op0 != NULL) code_instruction_operand_free(&(*instruction)->op0);
    if((*instruction)->op1 != NULL) code_instruction_operand_free(&(*instruction)->op1);
    if((*instruction)->op2 != NULL) code_instruction_operand_free(&(*instruction)->op2);
    memory_free(*instruction);
    *instruction = NULL;
}

