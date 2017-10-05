#include <stdio.h>
#include "memory.h"
#include "code_instruction.h"
#include "code_instruction_operand.h"
#include "debug.h"
#include "data_type.h"

CodeInstructionOperand* code_instruction_operand_init(TypeInstructionOperand type, CodeInstructionOperandData data) {
    CodeInstructionOperand* operand = memory_alloc(sizeof(CodeInstructionOperand));

    operand->type = type;
    operand->data = data;
    return operand;
}

void code_instruction_operand_free(CodeInstructionOperand** operand_) {
    NULL_POINTER_CHECK(operand_,);
    NULL_POINTER_CHECK(*operand_,);
    // TODO: free specific operands
    CodeInstructionOperand* operand = *operand_;
    switch(operand->type) {
        case TYPE_INSTRUCTION_OPERAND_CONSTANT:
        case TYPE_INSTRUCTION_OPERAND_VARIABLE:
        case TYPE_INSTRUCTION_OPERAND_LABEL:
        default:
            // TODO: free what?
            break;
    }
    memory_free(operand);
    *operand_ = NULL;
}

CodeInstructionOperand* code_instruction_operand_init_variable(SymbolVariable* variable) {
    NULL_POINTER_CHECK(variable, NULL);
    CodeInstructionOperandData data = {.variable=variable};
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_VARIABLE, data);
}

CodeInstructionOperand* code_instruction_operand_init_integer(int integer) {
    CodeInstructionOperandData data;
    data.constant.data.integer = integer;
    data.constant.data_type = DATA_TYPE_INTEGER;
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_CONSTANT, data);
}

CodeInstructionOperand* code_instruction_operand_init_double(double double_) {
    CodeInstructionOperandData data;
    data.constant.data.floating_point = double_;
    data.constant.data_type = DATA_TYPE_DOUBLE;
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_CONSTANT, data);
}

CodeInstructionOperand* code_instruction_operand_init_string(String* string) {
    NULL_POINTER_CHECK(string, NULL);
    CodeInstructionOperandData data;

    data.constant.data.string = *string;
    data.constant.data_type = DATA_TYPE_STRING;
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_CONSTANT, data);
}

CodeInstructionOperand* code_instruction_operand_init_label(const char* label) {
    NULL_POINTER_CHECK(label, NULL);
    CodeInstructionOperandData data;
    data.label = label;
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_LABEL, data);
}
