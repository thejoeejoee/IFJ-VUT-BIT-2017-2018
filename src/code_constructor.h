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

#define _GENERATE_STACK_DATA_TYPE_CONVERSION_CODE_2(operand_1_type, target_type) do{ \
        code_constructor_unary_operation_stack_type_conversion( \
            constructor, \
            (operand_1_type), \
            (target_type) \
        ); \
} while(false)

#define _GENERATE_STACK_DATA_TYPE_CONVERSION_CODE_3(operand_1_type, operand_2_type, target_type) do { \
    code_constructor_binary_operation_stack_type_conversion( \
        constructor, \
        (operand_1_type), \
        (operand_2_type), \
        (target_type), \
        parser->parser_semantic->temp_variable1 \
    ); \
} while(false)

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
    // for registering label loops to prepend variable declaration before cycle
    CodeInstruction* loops_initial_instruction;
    size_t loops_depth;

    // for generating labels in nested control statements
    size_t control_statement_depth;
    // for automatic generating conversion instructions
    LList* conversion_instructions;
    // internal
    size_t _label_counter;
} CodeConstructor;

/**
 * Costructor of code constructor.
 * @return
 */
CodeConstructor* code_constructor_init();

/**Constructor destructor
 * @param constructor
 */
void code_constructor_free(CodeConstructor** constructor);

/**
 * Add conversion instruction, for example:
 *  code_constructor_add_conversion_instruction(
            constructor, // instance
            I_INT_TO_FLOAT, // IFJcode17 instruction
            DATA_TYPE_INTEGER, DATA_TYPE_DOUBLE, // converts integer to double
            false // not on stack
    );
 *
 * @param constructor instance
 * @param instruction type of instruction
 * @param current_type source type
 * @param target_type target type
 * @param is_stack_instruction works on stack?
 */
void code_constructor_add_conversion_instruction(
        CodeConstructor* constructor,
        TypeInstruction instruction,
        DataType current_type,
        DataType target_type,
        bool is_stack_instruction
);

/**
 * Generate unique label for using in conditions, loops etc.
 * @param constructor instance
 * @param type type of label ("if_else", "if_else_if", etc.)
 * @return generated label
 */
char* code_constructor_generate_label(CodeConstructor* constructor, const char* type);

/**
 * Flush buffer with functions and skip it with jump-label.
 * @param constructor
 */
void code_constructor_end_code(CodeConstructor* constructor);

/**
 * Increment scope depth and for root scope also create local frame.
 * @param constructor instance
 */
void code_constructor_scope_start(CodeConstructor* constructor);

/**
 * Exits actual scope with decrementing scope counter.
 * @param constructor instance
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
 * Define SHARED variable in corresponding frame with correct data type.
 * @param constructor instance
 * @param symbol_variable variable
 */
void code_constructor_shared_variable_declaration(CodeConstructor* constructor, SymbolVariable* symbol_variable);

/**
 * Define STATIC variable in corresponding frame with correct data type. It also generates declaration guard.
 * @param constructor instance
 * @param symbol_variable static variable
 * @param function in function
 */
void code_constructor_static_variable_declaration(CodeConstructor* constructor, SymbolVariable* symbol_variable,
                                                  SymbolFunction* function);

/**
 * Ends static variable declaration guard.
 * @param constructor instance
 * @param symbol_variable static variable
 * @param function in function
 */
void code_constructor_static_variable_declaration_end(CodeConstructor* constructor, SymbolVariable* symbol_variable,
                                                      SymbolFunction* function);

/**
 * Read from stdin into given variable.
 * @param constructor instance
 * @param variable read into
 */
void code_constructor_input(CodeConstructor* constructor, SymbolVariable* variable);

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

/**
 * Print from given variable.
 * @param constructor constructor
 * @param print_variable variable to print
 */
void code_constructor_print_expression(CodeConstructor* constructor, SymbolVariable* print_variable);

/**
 * Generates code to assign result of expression to given variable.
 * @param constructor instance
 * @param variable target variable
 */
void code_constructor_variable_expression_assignment(CodeConstructor* constructor, SymbolVariable* variable);

/**
 * Generates label for starting function. It also enable buffer instruction generating.
 * @param constructor instance
 * @param function function to make header
 */
void code_constructor_function_header(CodeConstructor* constructor, SymbolFunction* function);

/**
 * Generates implicit return from function and. It also disable buffer instruction generating.
 * @param constructor instance
 * @param function function
 */
void code_constructor_function_end(CodeConstructor* constructor, SymbolFunction* function);

/**
 * Generates code from return from function.
 * @param constructor
 */
void code_constructor_return(CodeConstructor* constructor);

/**
 * Make implicit conversion on head of stack - for unary operations.
 * @param constructor instance
 * @param operand_type source type for conversion
 * @param target_type target type for conversion
 */
void code_constructor_unary_operation_stack_type_conversion(CodeConstructor* constructor, DataType operand_type,
                                                            DataType target_type);

/**
 * Make implicit conversion for two operands on stack - for binary operations.
 * @param constructor instance
 * @param operand_1_type source type of 1st operand
 * @param operand_2_type source type of 2nd operand
 * @param target_type target type for both operands
 * @param temp_var temp variable to defer stack value.
 */
void code_constructor_binary_operation_stack_type_conversion(CodeConstructor* constructor, DataType operand_1_type,
                                                             DataType operand_2_type, DataType target_type,
                                                             SymbolVariable* temp_var);

/**
 * Generates corresponding instruction to make conversion on stack - internal.
 * @param constructor instance
 * @param current_type source type
 * @param target_type target type
 * @internal
 */
void code_constructor_stack_type_conversion(CodeConstructor* constructor, DataType current_type, DataType target_type);

/**
 * Generates built-in function for string length using one temp variable.
 * @param constructor instance
 * @param tmp_variable temp variable
 * @param stack_param_type data type of argument
 */
void code_constructor_fn_length(CodeConstructor* constructor, SymbolVariable* tmp_variable, DataType stack_param_type);

/**
 * Generates built-in function for chr builtin function using one temp variable.
 * @param constructor instance
 * @param tmp_variable temp variable
 * @param stack_param_type data type of argument
 */
void code_constructor_fn_chr(CodeConstructor* constructor, SymbolVariable* tmp_variable, DataType param_type);

/**
 * Generates built-in function for asc using three temp variables.
 * @param constructor instance
 * @param tmp_variable1 tmp var 1
 * @param tmp_variable2 tmp var 2
 * @param tmp_variable3 tmp var 3
 * @param param_1_type first argument type
 * @param param_2_type second argument type
 */
void code_constructor_fn_asc(CodeConstructor* constructor, SymbolVariable* tmp_variable1, SymbolVariable* tmp_variable2,
                             SymbolVariable* tmp_variable3, DataType param_1_type, DataType param_2_type);

/**
 * Generates built-in function for substr using three temp variables.
 * @param constructor instance
 * @param tmp_variable1 tmp var 1
 * @param tmp_variable2 tmp var 2
 * @param tmp_variable3 tmp var 3
 * @param tmp_variable4 tmp var 4
 * @param tmp_variable5 tmp var 5
 * @param param_1_type first argument type
 * @param param_2_type second argument type
 * @param param_3_type third argument type
 */
void code_constructor_fn_substr(CodeConstructor* constructor, SymbolVariable* tmp1, SymbolVariable* tmp_variable2,
                                SymbolVariable* tmp_variable3, SymbolVariable* tmp_variable4,
                                SymbolVariable* tmp_variable5,
                                DataType param_1_type, DataType param_2_type, DataType param_3_type);

#endif //_CODE_CONSTRUCTOR_H
