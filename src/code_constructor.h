#ifndef _CODE_CONSTRUCTOR_H
#define _CODE_CONSTRUCTOR_H

#include "code_generator.h"
#include "stack_code_label.h"
#include "symtable_function.h"
#include "llist.h"

#define _GENERATE_CODE_1(name) _GENERATE_CODE_4(name, NULL, NULL, NULL)
#define _GENERATE_CODE_2(name, op0) _GENERATE_CODE_4(name, op0, NULL, NULL)
#define _GENERATE_CODE_3(name, op0, op1) _GENERATE_CODE_4(name, op0, op1, NULL)
#define _GENERATE_CODE_4(name, op0, op1, op2) code_generator_instruction(constructor->generator, name, op0, op1, op2)
#define GENERATE_CODE(...) MSVC_EXPAND(GET_OVERLOADED_MACRO1234(__VA_ARGS__, _GENERATE_CODE_4, _GENERATE_CODE_3, \
    _GENERATE_CODE_2, _GENERATE_CODE_1)(__VA_ARGS__))

#define _GENERATE_STACK_DATA_TYPE_CONVERSION_CODE_2(operand_1_type, target_type) SEMANTIC_ANALYSIS({ \
        code_constructor_unary_operation_stack_type_conversion( \
            constructor, \
            (operand_1_type), \
            (target_type) \
        ); \
})

#define _GENERATE_STACK_DATA_TYPE_CONVERSION_CODE_3(operand_1_type, operand_2_type, target_type) SEMANTIC_ANALYSIS({ \
    code_constructor_binary_operation_stack_type_conversion( \
        constructor, \
        (operand_1_type), \
        (operand_2_type), \
        (target_type), \
        parser->parser_semantic->temp_variable1 \
    ); \
})

#define GENERATE_STACK_DATA_TYPE_CONVERSION_CODE(...) MSVC_EXPAND(GET_OVERLOADED_MACRO123(__VA_ARGS__, _GENERATE_STACK_DATA_TYPE_CONVERSION_CODE_3, _GENERATE_STACK_DATA_TYPE_CONVERSION_CODE_2)(__VA_ARGS__))

typedef struct type_conversion_instruction_t {
    LListBaseItem base;
    TypeInstruction instruction;
    DataType current_type;
    DataType target_type;
    bool is_stack_instruction;
} TypeConversionInstruction;

typedef struct code_constructor_t {
    // generator for raw target code
    CodeGenerator* generator;

    // metadata about actually generated code
    Stack* code_label_stack;
    Stack* conditions_label_stack;
    Stack* loops_label_stack;
    size_t scope_depth;
    bool in_function_definition;

    size_t label_counter;
    size_t control_statement_depth;
    LList* conversion_instructions;
} CodeConstructor;

CodeConstructor* code_constructor_init();

void code_constructor_free(CodeConstructor** constructor);

void code_constructor_add_conversion_instruction(CodeConstructor* constructor, TypeInstruction instruction, DataType current_type, DataType target_type, bool is_stack_instruction);

char* code_constructor_generate_label(CodeConstructor* constructor, const char* type);

/**
 * Starts three address block with jump to main scope.
 * @param constructor
 */
void code_constructor_start_code(CodeConstructor* constructor);

/**
 * For root scope generates jump target into main program, always increment scope counter.
 */
void code_constructor_scope_start(CodeConstructor* constructor);

/**
 * Exits actual scope with decrementing scope counter.
 * @param constructor
 */
void code_constructor_scope_end(CodeConstructor* constructor);

/**
 * Define variable in corresponding frame with correct data type.
 * @param constructor
 * @param frame
 * @param symbol_variable
 */
void code_constructor_variable_declaration(CodeConstructor* constructor, SymbolVariable* symbol_variable);

/**
 * Read from stdin into given variable.
 * @param constructor
 * @param frame
 * @param symbolVariable
 */
void code_constructor_input(CodeConstructor* constructor, int frame, SymbolVariable* symbolVariable);

/**
 * Generates control block to handle expression result and maybe skip positive statements.
 * @param constructor
 */
void code_constructor_if_after_expression(CodeConstructor* constructor);

/**
 * Jump from positive block out from whole if.
 * @param constructor
 */
void code_constructor_if_end_if_block(CodeConstructor* constructor);

/**
 * Generates target for negative if jump.
 * @param constructor
 */
void code_constructor_if_else_if_before_expression(CodeConstructor* constructor);

/**
 * Resolve elseif condition and maybe skip to next branch.
 * @param constructor
 */
void code_constructor_if_else_if_after_expression(CodeConstructor* constructor);

/**
 * Generates jump from positive branch to end of whole if.
 * @param constructor
 */
void code_constructor_if_after_end_if(CodeConstructor* constructor);

/**
 * Generates label for target, but without any next condition.
 * @param constructor
 */
void code_constructor_if_else_block(CodeConstructor* constructor);

/**
 * Generates target for jumps from end of while.
 * @param constructor
 */
void code_constructor_while_before_condition(CodeConstructor* constructor);

/**
 * Resolves condition and maybe skip the while block.
 * @param constructor
 */
void code_constructor_while_after_condition(CodeConstructor* constructor);

/**
 * Generates target for skipping whole while block.
 * @param constructor
 */
void code_constructor_while_end(CodeConstructor* constructor);

void code_constructor_print_expression(CodeConstructor* constructor, SymbolVariable* print_variable);

void code_constructor_variable_expression_assignment(CodeConstructor* constructor, SymbolVariable* variable);

void code_constructor_generate_builtin_functions(CodeConstructor* constructor);

void code_constructor_function_header(CodeConstructor* constructor, SymbolFunction* function);

void code_constructor_function_end(CodeConstructor* constructor, SymbolFunction* function);

void code_constructor_return(CodeConstructor* constructor);

void code_constructor_stack_type_conversion(CodeConstructor* constructor, DataType current_type, DataType target_type);

void code_constructor_unary_operation_stack_type_conversion(CodeConstructor* constructor, DataType operand_1_type, DataType target_type);

void code_constructor_binary_operation_stack_type_conversion(CodeConstructor* constructor, DataType operand_1_type, DataType operand_2_type, DataType target_type, SymbolVariable* temp_var);

void code_constructor_fn_length(CodeConstructor* constructor, SymbolVariable* tmp_variable);

void code_constructor_fn_chr(CodeConstructor* constructor, SymbolVariable* tmp_variable);

#endif //_CODE_CONSTRUCTOR_H
