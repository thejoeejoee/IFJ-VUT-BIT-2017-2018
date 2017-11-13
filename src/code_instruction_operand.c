#include <stdio.h>
#include <ctype.h>
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
            if(operand->data.constant.data_type == DATA_TYPE_STRING)
                string_free(&(operand->data.constant.data.string));
            break;
        case TYPE_INSTRUCTION_OPERAND_VARIABLE:
            symbol_variable_free_data((SymbolTableBaseItem*) operand->data.variable);

            memory_free(operand->data.variable);
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
    CodeInstructionOperandData data = {.variable=symbol_variable_copy(variable)};
    NULL_POINTER_CHECK(data.variable, NULL);
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

    data.constant.data.string = string_copy(string);
    data.constant.data_type = DATA_TYPE_STRING;
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_CONSTANT, data);
}

CodeInstructionOperand* code_instruction_operand_init_label(const char* label) {
    NULL_POINTER_CHECK(label, NULL);
    CodeInstructionOperandData data;
    data.label = c_string_copy(label);
    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_LABEL, data);
}

CodeInstructionOperand* code_instruction_operand_init_data_type(DataType data_type) {
    ASSERT(data_type != DATA_TYPE_NONE);
    CodeInstructionOperandData data;
    data.constant.data_type = data_type;

    return code_instruction_operand_init(TYPE_INSTRUCTION_OPERAND_DATA_TYPE, data);
}

CodeInstructionOperand* code_instruction_operand_init_variable_from_param(SymbolFunction* function,
                                                                          SymbolFunctionParam* param) {
    NULL_POINTER_CHECK(function, NULL);
    NULL_POINTER_CHECK(param, NULL);

    SymbolVariable* variable = symbol_variable_init_from_function_param(function, param);

    CodeInstructionOperand* operand = code_instruction_operand_init_variable(variable);

    symbol_variable_single_free(&variable);
    return operand;
}

char* code_instruction_operand_render(CodeInstructionOperand* operand) {
    NULL_POINTER_CHECK(operand, NULL);

    size_t length = 1; // null terminator
    if(operand->type == TYPE_INSTRUCTION_OPERAND_CONSTANT && operand->data.constant.data_type == DATA_TYPE_STRING) {
        length += string_length(operand->data.constant.data.string) * 4;
    }
    else if(operand->type == TYPE_INSTRUCTION_OPERAND_LABEL)
        length += strlen(operand->data.label);

    else if(operand->type == TYPE_INSTRUCTION_OPERAND_VARIABLE) {
        if(operand->data.variable->alias_name == NULL)
            length += strlen(operand->data.variable->base.key);
        else
            length += strlen(operand->data.variable->alias_name);
    }

    length += 64; // data type + float&integers
    char* rendered = memory_alloc(sizeof(char) * length);
    char* escaped = NULL;
    switch(operand->type) {
        case TYPE_INSTRUCTION_OPERAND_LABEL:
            snprintf(rendered, length, "%s", operand->data.label);
            break;
        case TYPE_INSTRUCTION_OPERAND_DATA_TYPE:
            switch(operand->data.constant.data_type) {
                case DATA_TYPE_BOOLEAN:
                    snprintf(rendered, length, "bool");
                    break;
                case DATA_TYPE_DOUBLE:
                    snprintf(rendered, length, "float");
                    break;
                case DATA_TYPE_INTEGER:
                    snprintf(rendered, length, "int");
                    break;
                case DATA_TYPE_STRING:
                    snprintf(rendered, length, "string");
                    break;
                case DATA_TYPE_NONE:
                default:
                    LOG_WARNING("Unknown data type to render: %d.", operand->data.constant.data_type);
            }
            break;
        case TYPE_INSTRUCTION_OPERAND_VARIABLE: {
            code_instruction_operand_render_variable_identifier(operand->data.variable, rendered, length);
        }
            break;
        case TYPE_INSTRUCTION_OPERAND_CONSTANT:
            switch(operand->data.constant.data_type) {
                case DATA_TYPE_INTEGER:
                    snprintf(rendered, length, "int@%d", operand->data.constant.data.integer);
                    break;

                case DATA_TYPE_DOUBLE:
                    snprintf(rendered, length, "float@%g", operand->data.constant.data.double_);
                    break;

                case DATA_TYPE_BOOLEAN:
                    snprintf(rendered, length, "bool@%s", operand->data.constant.data.boolean ? "true" : "false");
                    break;

                case DATA_TYPE_STRING:
                    escaped = code_instruction_operand_escaped_string(operand->data.constant.data.string);
                    snprintf(rendered, length, "string@%s", escaped);
                    memory_free(escaped);
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

char* code_instruction_operand_escaped_string(String* source) {
    NULL_POINTER_CHECK(source, NULL);
    size_t source_length = string_length(source);
    String* escaped = string_init_with_capacity((size_t) (source_length * 4));
    short c;
    char buffer[5];

    for(size_t i = 0; i < source_length; ++i) {
        c = (unsigned char) source->content[i];
        if(isspace(c) || (c >= 0 && c <= 32) || c == 35 || c == 92 || c > 127) {
            snprintf(buffer, 4 + 1, "\\%03d", c);
            string_append_s(escaped, buffer);
        } else {
            string_append_c(escaped, (char) c);
        }
    }
    char* escaped_c_string = c_string_copy(string_content(escaped));
    string_free(&escaped);
    return escaped_c_string;
}

CodeInstructionOperand* code_instruction_operand_implicit_value(DataType data_type) {
    switch(data_type) {
        case DATA_TYPE_DOUBLE:
            return code_instruction_operand_init_double(0);
        case DATA_TYPE_INTEGER:
            return code_instruction_operand_init_integer(0);
        case DATA_TYPE_STRING: {
            String* string = string_init_with_capacity(0);
            return code_instruction_operand_init_string(string);
        }
        case DATA_TYPE_BOOLEAN:
            return code_instruction_operand_init_boolean(false);
        case DATA_TYPE_NONE:
            return NULL;
        default:
            LOG_WARNING("Unknown data type %d.", data_type);
            return NULL;
    }
}
void code_instruction_operand_render_variable_identifier(SymbolVariable* variable, char* rendered, size_t identifier_max_len)
{
    const char* frame = NULL;
    switch(variable->frame) {
        case VARIABLE_FRAME_LOCAL:
            frame = "LF";
            break;
        case VARIABLE_FRAME_GLOBAL:
            frame = "GF";
            if(variable->scope_depth > 0) {
                LOG_WARNING(
                        "Variable %s on global frame has non-zero scope depth: %zd.",
                        variable->base.key,
                        variable->scope_depth
                );
            }
            break;
        case VARIABLE_FRAME_TEMP:
            frame = "TF";
            if(variable->scope_depth == 0) {
                LOG_WARNING(
                        "Variable %s on temp frame (function parameter) has zero scope depth - invalid variable init.",
                        variable->base.key
                );
            }
            break;
        default:
            LOG_WARNING("Unknown variable frame %d.", variable->frame);
    }
    if(variable->scope_alias == NULL)
        snprintf(
                rendered,
                identifier_max_len,
                "%s@%%%05zd_%s",
                frame,
                variable->scope_depth,
                variable->alias_name == NULL ?
                variable->base.key : variable->alias_name
        );
    else
        snprintf(
                rendered,
                identifier_max_len,
                "%s@%%%s_%s",
                frame,
                variable->scope_alias,
                variable->alias_name == NULL ?
                variable->base.key : variable->alias_name
        );
}

size_t code_instruction_rendered_variable_identifier_max_len(SymbolVariable* variable)
{
    size_t length = 65;
    if(variable->alias_name == NULL)
        length += strlen(variable->base.key);
    else
        length += strlen(variable->alias_name);

    return length;
}

char* code_instruction_render_variable_identifier(SymbolVariable* variable)
{
    const size_t max_len = code_instruction_rendered_variable_identifier_max_len(variable);
    char* rendered = memory_alloc(max_len * sizeof(char));
    code_instruction_operand_render_variable_identifier(variable, rendered, max_len);

    return rendered;
}
