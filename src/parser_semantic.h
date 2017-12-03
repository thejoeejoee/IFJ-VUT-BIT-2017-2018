#ifndef _PARSER_SEMANTIC_H
#define _PARSER_SEMANTIC_H

#include "symbol_register.h"
#include "token.h"
#include "error.h"
#include "llist.h"
#include "parser_expr_internal.h"

typedef enum {
    SEMANTIC_ACTION__NONE,
    SEMANTIC_ACTION__FUNCTION_DECLARATION,
    SEMANTIC_ACTION__FUNCTION_DEFINITION,
    SEMANTIC_ACTION__VARIABLE_ASSIGNMENT
} SemanticAction;

typedef enum {
    OPERATION_ADD,
    OPERATION_SUB,
    OPERATION_MULTIPLY,
    OPERATION_INT_DIVIDE,
    OPERATION_DIVIDE,

    // Bool operations

    OPERATION_NOT,
    OPERATION_AND,
    OPERATION_OR,

    OPERATION_GREATER,
    OPERATION_GREATER_OR_EQUAL,
    OPERATION_LESSER,
    OPERATION_LESSER_OR_EQUAL,
    OPERATION_EQUAL,
    OPERATION_NOT_EQUAL,

    OPERATION_IMPLICIT_CONVERSION,
    OPERATION_UNARY_MINUS,

    OPERATION__LAST
} TypeExpressionOperation;

typedef struct expr_operation_signature_t {
    LListBaseItem base;
    TypeExpressionOperation operation_type;
    DataType operand_1_type;
    DataType operand_2_type;
    DataType conversion_target_type;
    DataType result_type;
} OperationSignature;

typedef struct parser_semantic_t {
    SymbolRegister* register_;

    ErrorReport error_report;

    SemanticAction actual_action;

    // metadata
    SymbolFunction* actual_function; // Pointer to actual function
    SymbolVariable* actual_variable; // Pointer to actual variable
    size_t argument_index;
    // TODO: is was_actual_function_declared needed here? why is not controlled by scenarios or actual_function->declared?
    bool was_actual_function_declared;
    SymbolVariable* temp_variable1;
    SymbolVariable* temp_variable2;
    SymbolVariable* temp_variable3;
    SymbolVariable* temp_variable4;
    SymbolVariable* temp_variable5;
    SymbolVariable* temp_variable6;
    LList* operations_signatures[OPERATION__LAST];
    ExprToken* expression_result;
} ParserSemantic;

/**
 * Constructor for parser_semantic
 *
 * @return ParserSematic*
 */
ParserSemantic* parser_semantic_init();

/**
 * @brief Destructor for parser semantic
 *
 * @param ParserSemantic** parser
 */
void parser_semantic_free(ParserSemantic** parser);

/**
 * @brief Set actual action for parser semantic. Sematic actions are scenarios. We have to keep the action.
 *
 * @param parser_semantic
 * @param actual_action
 */
void parser_semantic_set_action(ParserSemantic* parser_semantic, SemanticAction actual_action);

/**
 * @brief Sets the function name for the current scenario.
 *
 * @param parser_semantic
 * @param char* name Name of function
 * @return bool was successfully sets?
 */
bool parser_semantic_set_function_name(ParserSemantic* parser_semantic, char* name);


/**
 * @brief Add Variable to the actual symbol table
 *
 * @param parser_semantic instance
 * @param name name of variable
 * @param data_type data type of variable
 * @return instance of created variable
 */
SymbolVariable* parser_semantic_add_variable(ParserSemantic* parser_semantic, char* name, DataType data_type);

/**
 * @brief If the actual action is ACTUAL_ACTION__FUNCTION_DECLARATION, then set return data type for actual function,
 *        If the actual action is ACTUAL_ACTION__FUNCTION_DEFINITION, then check return data type in symbol table
 * @param parser_semantic instance
 * @param data_type return data type to set
 * @return bool was successfully sets?
 */
bool parser_semantic_set_function_return_data_type(ParserSemantic* parser_semantic, DataType data_type);

/**
 *  Add function parameter to list of params.
 * @param parser_semantic instant
 * @param name parameter name
 * @param data_type data types
 * @return bool was successfully sets?
 */
bool parser_semantic_add_function_parameter(ParserSemantic* parser_semantic, char* name, DataType data_type);

/**
 *  Checks function parameter count with declaration.
 * @param parser_semantic instance
 * @return true if was check successful
 */
bool parser_semantic_check_count_of_function_arguments(ParserSemantic* parser_semantic);

/**
 * Returns false if exists any declared function without definition.
 * @param parser_semantic instance
 * @return result of check
 */
bool parser_semantic_check_function_definitions(ParserSemantic* parser_semantic);

/**
 * Initialize all params into new variables table.
 * @param parser_semantic instance
 * @param function starting function
 */
void parser_semantic_function_start(ParserSemantic* parser_semantic, SymbolFunction* function);

/**
 * Pop variables table as ending function block.
 * @param parser_semantic instance
 */
void parser_semantic_function_end(ParserSemantic* parser_semantic);

/**
 * Add new operation signature into signatures register.
 * @param parser_semantic instance
 * @param operation type of operation
 * @param operand_1_type first operand type
 * @param operand_2_type second operand type
 * @param target_type step data types to convert input operands
 * @param result_type result data type
 */
void parser_semantic_add_operation_signature(
        ParserSemantic* parser_semantic, TypeExpressionOperation operation,
        DataType operand_1_type, DataType operand_2_type, DataType target_type,
        DataType result_type
);

/**
 * Gets Signature or NULL if operand types and result type were matched.
 * @param parser_semantic instance
 * @param operation_type operation type
 * @param operand_1_type first operand type
 * @param operand_2_type second operand type
 * @param target_type target type of operation
 * @return NULL if was signature found
 */
OperationSignature* parser_semantic_get_operation_signature(
        ParserSemantic* parser_semantic,
        TypeExpressionOperation operation_type,
        DataType operand_1_type, DataType operand_2_type,
        DataType target_type
);

/**
 * Gets conversion data type to operands based by two operands and target conversion type.
 * @param parser_semantic instance
 * @param operation_type type of operation
 * @param operand_1_type first operand type
 * @param operand_2_type second operand type
 * @param target_conversion_type conversion type
 * @return data type of needed conversion type
 */
DataType parser_semantic_resolve_implicit_data_type_conversion(
        ParserSemantic* parser_semantic,
        TypeExpressionOperation operation_type,
        DataType operand_1_type, DataType operand_2_type,
        DataType target_conversion_type
);

/**
 * Setups all temp variables used for writes/reads and implementation of builtin functions.
 * @param parser_semantic instance
 */
void parser_semantic_setup_temp_variables(ParserSemantic* parser_semantic);

#endif //_PARSER_SEMANTIC_H
