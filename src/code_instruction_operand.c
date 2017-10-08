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
            break;
        case TYPE_INSTRUCTION_OPERAND_LABEL:
            memory_free((void*) operand->data.label);
            operand->data.label = NULL;
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
    data.constant.data.double_ = double_;
    data.constant.data_type = DATA_TYPE_DOUBLE;
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_CONSTANT, data);
}

CodeInstructionOperand* code_instruction_operand_init_boolean(bool boolean) {
    CodeInstructionOperandData data;
    data.constant.data.boolean = boolean;
    data.constant.data_type = DATA_TYPE_BOOLEAN;
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
    data.label = c_string_copy(label);
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_LABEL, data);
}

char* code_instruction_operand_render(CodeInstructionOperand* operand) {
    if(operand == NULL)
        return "";

    size_t length = 1;
    if(operand->type == TYPE_INSTRUCTION_OPERAND_CONSTANT && operand->data.constant.data_type == DATA_TYPE_STRING) {
        length += string_length(&(operand->data.constant.data.string));
    } else {
        length += 7;
    }
    char* rendered = memory_alloc(sizeof(char) * length);
    switch(operand->type) {
        case TYPE_INSTRUCTION_OPERAND_LABEL:
            snprintf(rendered, length, "%s", operand->data.label);
            break;
        case TYPE_INSTRUCTION_OPERAND_SYMBOL:
            // TODO: resolve frame
            snprintf(rendered, length, "GF@%s", operand->data.variable->base.key);
            break;
        case TYPE_INSTRUCTION_OPERAND_CONSTANT:
            switch(operand->data.constant.data_type) {
                case DATA_TYPE_INTEGER:
                    snprintf(rendered, length, "int@% d", operand->data.constant.data.integer);
                    break;

                case DATA_TYPE_DOUBLE:
                    snprintf(rendered, length, "float@% g", operand->data.constant.data.double_);
                    break;

                case DATA_TYPE_BOOLEAN:
                    snprintf(rendered, length, "bool@%s", operand->data.constant.data.boolean ? "true" : "false");
                    break;

                case DATA_TYPE_STRING:
                    snprintf(rendered, length, "string@%s", string_content(&(operand->data.constant.data.string)));
                    break;
                default:
                    LOG_WARNING("Unknown operand data type.");
                    break;
            }
            break;
        default:
            LOG_WARNING("Unknown operand.");
            break;
    }
    return rendered;
}
