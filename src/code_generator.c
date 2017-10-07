#include "code_generator.h"
#include "memory.h"
#include "common.h"
#include "debug.h"

CodeGenerator* code_generator_init() {
    CodeGenerator* generator = memory_alloc(sizeof(CodeGenerator));

    generator->first = generator->last = NULL;

    return generator;
}

void code_generator_free(CodeGenerator** generator) {
    NULL_POINTER_CHECK(generator,);
    NULL_POINTER_CHECK(*generator,);
    CodeInstruction* instruction = (*generator)->first;
    CodeInstruction* next = NULL;
    while(instruction != NULL) {
        next = instruction->next;
        code_instruction_free(&instruction);
        instruction = next;
    }
    memory_free(*generator);
    *generator = NULL;
}

void code_generator_generate_instruction(
        CodeGenerator* generator,
        TypeInstruction type_instruction,
        CodeInstructionOperand* op0,
        CodeInstructionOperand* op1,
        CodeInstructionOperand* op2
) {
    NULL_POINTER_CHECK(generator,);
    CodeInstruction* instruction = code_instruction_init(type_instruction, op0, op1, op2);

    if(generator->last == NULL) {
        generator->last = generator->first = instruction;
        instruction->prev = instruction->next = NULL;
    } else {
        generator->last->next = instruction;
        instruction->prev = generator->last;
        generator->last = instruction;
    }
}

bool code_generator_generic_instruction(
        CodeGenerator* generator,
        TypeInstruction type_instruction,
        CodeInstructionOperand* op0, TypeInstructionOperand type0,
        CodeInstructionOperand* op1, TypeInstructionOperand type1,
        CodeInstructionOperand* op2, TypeInstructionOperand type2
) {
    if(type0)
        CHECK_OPERAND(op0, type0);
    // TODO: possible mem leak for type1 or type2 type check fail
    if(type1)
        CHECK_OPERAND(op1, type1);
    if(type2)
        CHECK_OPERAND(op2, type2);

    code_generator_generate_instruction(generator, type_instruction, op0, op1, op2);
    return true;
}

CODE_GENERATE_METHOD(I_DEF_VAR)

CODE_GENERATE_METHOD(I_WRITE, TYPE_INSTRUCTION_OPERAND_SYMBOL)

CODE_GENERATE_METHOD(I_LABEL, TYPE_INSTRUCTION_OPERAND_LABEL)

CODE_GENERATE_METHOD(I_JUMP, TYPE_INSTRUCTION_OPERAND_LABEL)

CODE_GENERATE_METHOD(I_JUMP_IF_EQUAL,
                     TYPE_INSTRUCTION_OPERAND_LABEL, TYPE_INSTRUCTION_OPERAND_SYMBOL, TYPE_INSTRUCTION_OPERAND_SYMBOL)