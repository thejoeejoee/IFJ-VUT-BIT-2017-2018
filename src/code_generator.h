#ifndef _CODE_GENERATOR_H
#define _CODE_GENERATOR_H

#include "debug.h"
#include "memory.h"
#include "code_instruction.h"

#define CHECK_OPERAND(op, operand_type) do { \
    NULL_POINTER_CHECK(op, false); \
    ASSERT(((op)->type & (operand_type))); \
    if (!(((op)->type & (operand_type)))) { code_instruction_operand_free(&(op)); return false; } \
} while(0)

#define CODE_GENERATE_METHOD_HEADER_0(name) bool code_generate_##name(CodeGenerator* generator)
#define CODE_GENERATE_METHOD_HEADER_1(name) bool code_generate_##name(CodeGenerator* generator, \
    CodeInstructionOperand* op0)
#define CODE_GENERATE_METHOD_HEADER_2(name) bool code_generate_##name( \
    CodeGenerator* generator, CodeInstructionOperand* op0, CodeInstructionOperand* op1)
#define CODE_GENERATE_METHOD_HEADER_3(name) bool code_generate_##name( \
    CodeGenerator* generator, CodeInstructionOperand* op0, CodeInstructionOperand* op1, CodeInstructionOperand* op2)

#define _GENERATE_METHOD_1(name) CODE_GENERATE_METHOD_HEADER_0(name) { \
    return code_generator_generic_instruction(generator, name, NULL, 0, NULL, 0, NULL, 0); \
}
#define _GENERATE_METHOD_2(name, type0) CODE_GENERATE_METHOD_HEADER_1(name) { \
    return code_generator_generic_instruction(generator, name, op0, type0, NULL, 0, NULL, 0); \
}
#define _GENERATE_METHOD_3(name, type0, type1) CODE_GENERATE_METHOD_HEADER_2(name) { \
    return code_generator_generic_instruction(generator, name, op0, type0, op1, type1, NULL, 0); \
}
#define _GENERATE_METHOD_4(name, type0, type1, type2) CODE_GENERATE_METHOD_HEADER_3(name) { \
    return code_generator_generic_instruction(generator, name, op0, type0, op1, type1, op2, type2); \
}
#define CODE_GENERATE_METHOD(...) MSVC_EXPAND(GET_OVERLOADED_MACRO1234(__VA_ARGS__, _GENERATE_METHOD_4, \
    _GENERATE_METHOD_3, _GENERATE_METHOD_2, _GENERATE_METHOD_1)(__VA_ARGS__))

typedef struct code_generator_t {
    CodeInstruction* first;
    CodeInstruction* last;
} CodeGenerator;

CodeGenerator* code_generator_init();

void code_generator_free(CodeGenerator** generator);

/**
 * Raw method for adding new, already checked, instructions.
 * @param generator instance
 * @param type_instruction type of new instruction
 * @param op0 optionally operand
 * @param op1 optionally operand
 * @param op2 optionally operand
 * @internal
 */
void code_generator_generate_instruction(
        CodeGenerator* generator,
        TypeInstruction type_instruction,
        CodeInstructionOperand* op0,
        CodeInstructionOperand* op1,
        CodeInstructionOperand* op2
);

/**
 * Generic target for all generated methods for
 * @param generator instance
 * @param type_instruction type instruction
 * @param op0 optionally operand
 * @param type0 0 for forbidden operand, else bit mask for check op type
 * @param op1 optionally operand
 * @param type1 0 for forbidden operand, else bit mask for check op type
 * @param op2 optionally operand
 * @param type2 0 for forbidden operand, else bit mask for check op type
 * @internal
 * @return true for success validation, else false
 */
bool code_generator_generic_instruction(
        CodeGenerator* generator,
        TypeInstruction type_instruction,
        CodeInstructionOperand* op0,
        TypeInstructionOperand type0,
        CodeInstructionOperand* op1,
        TypeInstructionOperand type1,
        CodeInstructionOperand* op2,
        TypeInstructionOperand type2
);

CODE_GENERATE_METHOD_HEADER_1(I_WRITE);

CODE_GENERATE_METHOD_HEADER_0(I_DEF_VAR);

CODE_GENERATE_METHOD_HEADER_1(I_LABEL);

CODE_GENERATE_METHOD_HEADER_1(I_JUMP);

CODE_GENERATE_METHOD_HEADER_3(I_JUMP_IF_EQUAL);


#endif //_CODE_GENERATOR_H
