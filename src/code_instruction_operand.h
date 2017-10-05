#ifndef _CODE_INSTRUCTION_OPERAND_H
#define _CODE_INSTRUCTION_OPERAND_H

#include <stdbool.h>
#include "symtable.h"
#include "dynamic_string.h"

typedef enum type_instruction_operand_t {
    TYPE_INSTRUCTION_OPERAND_VARIABLE = 1,
    TYPE_INSTRUCTION_OPERAND_CONSTANT = 2,
    TYPE_INSTRUCTION_OPERAND_LABEL = 4,

    TYPE_INSTRUCTION_OPERAND_SYMBOL = TYPE_INSTRUCTION_OPERAND_VARIABLE | TYPE_INSTRUCTION_OPERAND_CONSTANT, // symbol
} TypeInstructionOperand;

typedef struct code_instruction_operand_constant_data_t {
    union {
        int integer;
        bool boolean;
        double floating_point;
        String string;
    } data;
    DataType data_type;
} CodeInstructionOperandConstantData;

typedef union {
    // variable specific
    SymbolVariable* variable;

    // constant specific
    CodeInstructionOperandConstantData constant;

    // label specific
    const char* label;
} CodeInstructionOperandData;

typedef struct code_instruction_operand_t {
    TypeInstructionOperand type;
    CodeInstructionOperandData data;
} CodeInstructionOperand;

CodeInstructionOperand* code_instruction_operand_init_integer(int integer);

CodeInstructionOperand* code_instruction_operand_init_string(String* string);

CodeInstructionOperand* code_instruction_operand_init_double(double floating_point);

CodeInstructionOperand* code_instruction_operand_init_label(const char* label);

CodeInstructionOperand* code_instruction_operand_init_variable(SymbolVariable* variable);

CodeInstructionOperand* code_instruction_operand_init(TypeInstructionOperand type, CodeInstructionOperandData data);

void code_instruction_operand_free(CodeInstructionOperand** operand);

#endif //_CODE_INSTRUCTION_OPERAND_H
