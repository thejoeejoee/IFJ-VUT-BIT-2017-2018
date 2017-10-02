#include "code_generator.h"
#include "memory.h"

void generate(CodeGenerator* pGenerator, enum type_instruction_t instruction, CodeInstructionOperand* pOperand,
              CodeInstructionOperand* pInstruction_operand, CodeInstructionOperand* pInstructionOperand);

CodeGenerator* code_generator_init() {
    CodeGenerator* generator = memory_alloc(sizeof(CodeGenerator));

    generator->first = generator->last = NULL;

    return generator;
}


void generate_ADD(CodeGenerator* generator,
                  CodeInstructionOperand* op0,
                  CodeInstructionOperand* op1,
                  CodeInstructionOperand* op2) {

    ASSERT(op0->type == TYPE_INSTRUCTION_OPERAND_VARIABLE);

    generate(generator, I_ADD, op0, op1, op2);
}

void generate_INT2FLOAT(CodeGenerator* generator,
                        CodeInstructionOperand* op0,
                        CodeInstructionOperand* op1) {

    ASSERT(op0->type == TYPE_INSTRUCTION_OPERAND_VARIABLE);

    generate(generator, I_ADD, op0, op1, NULL);
}

void generate(CodeGenerator* pGenerator, enum type_instruction_t instruction, CodeInstructionOperand* pOperand,
              CodeInstructionOperand* pInstruction_operand, CodeInstructionOperand* pInstructionOperand
) {


}