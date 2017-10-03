#ifndef _CODE_INSTRUCTION_OPERAND_H
#define _CODE_INSTRUCTION_OPERAND_H

#include <stdbool.h>

typedef enum type_instruction_operand_t {
    TYPE_INSTRUCTION_OPERAND_VARIABLE,
    TYPE_INSTRUCTION_OPERAND_SYMBOL,
    TYPE_INSTRUCTION_OPERAND_LABEL,
} TypeInstructionOperand;

typedef union code_instruction_operand_data_t {
    int integer;
    bool boolean;
    double floating_point;
    char* string;
} CodeInstructionOperandData;

typedef struct code_instruction_operand_t {
    TypeInstructionOperand type;
    // variable specific
    char* identifier; // TODO: only identifier name? or ptr to symbol table?

    // constant specific
    short data_type; // TODO: add enum for data types
    CodeInstructionOperandData data;

    // label specific
    char* label; // TODO: only label name? or standalone struct with additional info?
} CodeInstructionOperand;

void code_instruction_operand_free(CodeInstructionOperand** operand);

#endif //_CODE_INSTRUCTION_OPERAND_H
