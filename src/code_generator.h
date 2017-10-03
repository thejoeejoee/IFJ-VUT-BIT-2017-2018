#ifndef _CODE_GENERATOR_H
#define _CODE_GENERATOR_H

#include "debug.h"
#include "memory.h"
#include "code_instruction.h"

typedef struct code_generator_t {
    CodeInstruction* first;
    CodeInstruction* last;
} CodeGenerator;

CodeGenerator* code_generator_init();

void code_generator_free(CodeGenerator** generator);

void code_generator_generate_instruction(
        CodeGenerator* generator,
        TypeInstruction type_instruction,
        CodeInstructionOperand* op0,
        CodeInstructionOperand* op1,
        CodeInstructionOperand* op2
);

#endif //_CODE_GENERATOR_H
