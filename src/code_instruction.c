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

char* code_instruction_render(CodeInstruction* instruction) {
    NULL_POINTER_CHECK(instruction, NULL);
    char* op0 = NULL;
    char* op1 = NULL;
    char* op2 = NULL;
    size_t length = 0;
    if(instruction->op0) {
        op0 = code_instruction_operand_render(instruction->op0);
        length += strlen(op0);
    }
    if(instruction->op1) {
        op1 = code_instruction_operand_render(instruction->op1);
        length += strlen(op1);
    }
    if(instruction->op2) {
        op2 = code_instruction_operand_render(instruction->op2);
        length += strlen(op2);
    }

    char* formatted = (char*) memory_alloc(sizeof(char) * (length + 16));
    snprintf(
            formatted,
            length - 1,
            "%d %s %s %s",
            instruction->type,
            op0,
            op1,
            op2
    );
    if(op0 != NULL)
        memory_free(op0);
    if(op1 != NULL)
        memory_free(op1);
    if(op2 != NULL)
        memory_free(op2);
    return formatted;
}

