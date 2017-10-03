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

void code_instruction_operand_free(CodeInstructionOperand** operand) {
    NULL_POINTER_CHECK(operand,);
    NULL_POINTER_CHECK(*operand,);
    // TODO: free operand with string data
}

CodeInstructionOperand* code_instruction_operand_init_variable(SymbolVariable* variable) {
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

CodeInstructionOperand* code_instruction_operand_init_string(const char* string) {
    CodeInstructionOperandData data;

    String* dynamic_string = string_init_with_capacity(strlen(string) + 1);
    string_append_s(dynamic_string, string);
    data.constant.data.string = *dynamic_string;
    string_free(&dynamic_string);

    data.constant.data_type = DATA_TYPE_STRING;
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_CONSTANT, data);
}

CodeInstructionOperand* code_instruction_operand_init_label(const char* label) {
    CodeInstructionOperandData data;
    data.label = label;
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_LABEL, data);
}
