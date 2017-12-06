#ifndef _CODE_INSTRUCTION_OPERAND_H
#define _CODE_INSTRUCTION_OPERAND_H

#include <stdbool.h>
#include "symtable.h"
#include "dynamic_string.h"
#include "data_type.h"
#include "symtable_variable.h"
#include "symtable_function.h"

#define OPERANDS_MAX_COUNT 3

typedef enum type_instruction_operand_t {
    TYPE_INSTRUCTION_OPERAND_NONE = 0,

    TYPE_INSTRUCTION_OPERAND_VARIABLE = 1,
    TYPE_INSTRUCTION_OPERAND_CONSTANT = 2,
    TYPE_INSTRUCTION_OPERAND_LABEL = 4,
    TYPE_INSTRUCTION_OPERAND_DATA_TYPE = 8,

    TYPE_INSTRUCTION_OPERAND_SYMBOL = TYPE_INSTRUCTION_OPERAND_VARIABLE | TYPE_INSTRUCTION_OPERAND_CONSTANT, // symbol
} TypeInstructionOperand;

typedef struct code_instruction_operand_constant_data_t {
    union {
        int integer;
        bool boolean;
        double double_;
        String* string;
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

/**
 * Copy instruction operand
 * @param other to copy
 * @return new instance
 */
CodeInstructionOperand* code_instruction_operand_copy(CodeInstructionOperand* other);
/**
 * Compare two operands with their values.
 * @param first to compare
 * @param second to compare
 * @return equality
 */
bool code_instruction_operand_cmp(CodeInstructionOperand* first, CodeInstructionOperand* second);

/**
 * New operand from integer value.
 * @param integer value
 * @return new operand
 */
CodeInstructionOperand* code_instruction_operand_init_integer(int integer);

/**
 * New operand from string value.
 * @param string value
 * @return new operand
 */
CodeInstructionOperand* code_instruction_operand_init_string(String* string);

/**
 * New operand from floating_point value.
 * @param floating_point value
 * @return new operand
 */
CodeInstructionOperand* code_instruction_operand_init_double(double floating_point);

/**
 * New operand from boolean value.
 * @param boolean value
 * @return new operand
 */
CodeInstructionOperand* code_instruction_operand_init_boolean(bool boolean);

/**
 * New operand from label value.
 * @param label value
 * @return new operand
 */
CodeInstructionOperand* code_instruction_operand_init_label(const char* label);

/**
 * New operand from label value.
 * @param label value
 * @return new operand
 */
CodeInstructionOperand* code_instruction_operand_init_variable(SymbolVariable* label);

/**
 * New operand from data_type value.
 * @param data_type value
 * @return new operand
 */
CodeInstructionOperand* code_instruction_operand_init_data_type(DataType data_type);

/**
 * New operand from function param value.
 * @param function function for param
 * @param param param
 * @return new operand
 */
CodeInstructionOperand* code_instruction_operand_init_variable_from_param(SymbolFunction* function,
                                                                          SymbolFunctionParam* param);

/**
 * Generic target for all operand constructors.
 * @param type type of operand
 * @param data data
 * @internal
 * @return new operand
 */
CodeInstructionOperand* code_instruction_operand_init(TypeInstructionOperand type, CodeInstructionOperandData data);

char* code_instruction_operand_escaped_string(String* source);

void code_instruction_operand_free(CodeInstructionOperand** operand);

char* code_instruction_operand_render(CodeInstructionOperand* operand);

char* code_instruction_render_variable_identifier(SymbolVariable* variable);

char* variable_cached_identifier(SymbolVariable* variable);

size_t code_instruction_rendered_variable_identifier_max_len(SymbolVariable* variable);

void code_instruction_operand_render_variable_identifier(SymbolVariable* variable, char* rendered,
                                                         size_t identifier_max_len);

CodeInstructionOperand* code_instruction_operand_implicit_value(DataType data_type);

#endif //_CODE_INSTRUCTION_OPERAND_H
