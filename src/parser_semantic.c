#include "parser_semantic.h"


ParserSemantic* parser_semantic_init() {
    ParserSemantic* parser_semantic = memory_alloc(sizeof(ParserSemantic));

    parser_semantic->register_ = symbol_register_init();
    parser_semantic->actual_action = SEMANTIC_ACTION__NONE;
    parser_semantic->actual_variable = NULL;
    parser_semantic->actual_function = NULL;
    parser_semantic->temp_variable1 = NULL;
    parser_semantic->temp_variable2 = NULL;
    parser_semantic->temp_variable3 = NULL;
    parser_semantic->error_report.error_code = ERROR_NONE;

    // Add allowed operations signatures
    for(int i = 0; i < (int) OPERATION__LAST; i++)
        llist_init(&(parser_semantic->operations_signatures[i]), sizeof(OperationSignature), NULL, NULL, NULL);

    // Operation add signatures
    // MATH OPERATIONS
    parser_semantic_add_operation_signature(parser_semantic, OPERATION_ADD,
                                            DATA_TYPE_INTEGER, DATA_TYPE_INTEGER,
                                            DATA_TYPE_INTEGER, DATA_TYPE_INTEGER);
    parser_semantic_add_operation_signature(parser_semantic, OPERATION_ADD,
                                            DATA_TYPE_INTEGER, DATA_TYPE_DOUBLE,
                                            DATA_TYPE_DOUBLE, DATA_TYPE_DOUBLE);
    parser_semantic_add_operation_signature(parser_semantic, OPERATION_ADD,
                                            DATA_TYPE_STRING, DATA_TYPE_STRING,
                                            DATA_TYPE_STRING, DATA_TYPE_STRING);

    parser_semantic_add_operation_signature(parser_semantic, OPERATION_SUB,
                                            DATA_TYPE_INTEGER, DATA_TYPE_INTEGER,
                                            DATA_TYPE_INTEGER, DATA_TYPE_INTEGER);

    parser_semantic_add_operation_signature(parser_semantic, OPERATION_SUB,
                                            DATA_TYPE_DOUBLE, DATA_TYPE_DOUBLE,
                                            DATA_TYPE_DOUBLE, DATA_TYPE_DOUBLE);

    parser_semantic_add_operation_signature(parser_semantic, OPERATION_SUB,
                                            DATA_TYPE_INTEGER, DATA_TYPE_DOUBLE,
                                            DATA_TYPE_DOUBLE, DATA_TYPE_DOUBLE);

    // TODO add boolean, string, double in greater operation
    const TypeExpressionOperation compare_operations[] = {
            OPERATION_GREATER,
            OPERATION_GREATER_OR_EQUAL,
            OPERATION_EQUAL,
            OPERATION_LESSER_OR_EQUAL,
            OPERATION_LESSER,
    };
    // comparing
    for(int j = 0; j < (sizeof(compare_operations) / sizeof(*compare_operations)); ++j) {
        parser_semantic_add_operation_signature(parser_semantic, compare_operations[j],
                                                DATA_TYPE_DOUBLE, DATA_TYPE_INTEGER,
                                                DATA_TYPE_DOUBLE, DATA_TYPE_BOOLEAN);
        parser_semantic_add_operation_signature(parser_semantic, compare_operations[j],
                                                DATA_TYPE_DOUBLE, DATA_TYPE_DOUBLE,
                                                DATA_TYPE_DOUBLE, DATA_TYPE_BOOLEAN);
        parser_semantic_add_operation_signature(parser_semantic, compare_operations[j],
                                                DATA_TYPE_INTEGER, DATA_TYPE_INTEGER,
                                                DATA_TYPE_INTEGER, DATA_TYPE_BOOLEAN);
        parser_semantic_add_operation_signature(parser_semantic, compare_operations[j],
                                                DATA_TYPE_STRING, DATA_TYPE_STRING,
                                                DATA_TYPE_STRING, DATA_TYPE_BOOLEAN);
    }

    return parser_semantic;
}

void parser_semantic_free(ParserSemantic** parser) {
    NULL_POINTER_CHECK(parser,);
    NULL_POINTER_CHECK(*parser,);

    for(int i = 0; i < (int) OPERATION__LAST; i++)
        llist_free(&((*parser)->operations_signatures[i]));

    symbol_register_free(&(*parser)->register_);
    memory_free(*parser);
    *parser = NULL;
}

void parser_semantic_set_action(ParserSemantic* parser_semantic, SemanticAction actual_action) {
    NULL_POINTER_CHECK(parser_semantic,);
    parser_semantic->actual_action = actual_action;

    if(actual_action == SEMANTIC_ACTION__FUNCTION_DEFINITION)
        parser_semantic->argument_index = 0;
}

SymbolVariable* parser_semantic_expect_symbol_variable(ParserSemantic* parser_semantic, Token token) {
    NULL_POINTER_CHECK(parser_semantic, NULL);
    NULL_POINTER_CHECK(token.data, NULL);
    SymbolVariable* symbol_variable = symbol_register_find_variable_recursive(
            parser_semantic->register_,
            token.data
    );
    if(symbol_variable == NULL) {
        parser_semantic->error_report.error_code = ERROR_SEMANTIC_DEFINITION;
    }

    return symbol_variable;
}

SymbolVariable* parser_semantic_add_symbol_variable(ParserSemantic* parser_semantic, char* name, DataType data_type) {
    NULL_POINTER_CHECK(parser_semantic, false);
    NULL_POINTER_CHECK(name, false);

    if(symbol_table_function_get(parser_semantic->register_->functions, name) != NULL) {
        parser_semantic->error_report.error_code = ERROR_SEMANTIC_DEFINITION;
        return false;
    }

    if(symbol_register_find_variable(parser_semantic->register_, name) != NULL) {
        // already declared in current scope
        parser_semantic->error_report.error_code = ERROR_SEMANTIC_DEFINITION;
        return false;
    }

    SymbolVariable* symbol_variable = symbol_register_new_variable(
            parser_semantic->register_,
            name
    );

    symbol_variable->data_type = data_type;
    return symbol_variable;
}

bool parser_semantic_set_function_name(ParserSemantic* parser_semantic, char* name) {
    NULL_POINTER_CHECK(parser_semantic, false);
    NULL_POINTER_CHECK(name, false);

    if(parser_semantic->actual_action == SEMANTIC_ACTION__FUNCTION_DECLARATION) {
        if(symbol_table_function_get(parser_semantic->register_->functions, name) != NULL) {
            parser_semantic->error_report.error_code = ERROR_SEMANTIC_DEFINITION;
            return false;
        }

        parser_semantic->actual_function = symbol_table_function_get_or_create(
                parser_semantic->register_->functions,
                name
        );

        parser_semantic->actual_function->arguments_count = 0;
        parser_semantic->actual_function->declared = true;

    } else if(parser_semantic->actual_action == SEMANTIC_ACTION__FUNCTION_DEFINITION) {

        if(symbol_table_function_get(parser_semantic->register_->functions, name) == NULL)
            parser_semantic->function_declared = false;
        else
            parser_semantic->function_declared = true;

        parser_semantic->actual_function = symbol_table_function_get_or_create(
                parser_semantic->register_->functions,
                name
        );

        if(parser_semantic->actual_function->defined) {
            parser_semantic->error_report.error_code = ERROR_SEMANTIC_DEFINITION;
            return false;
        }

        if(!parser_semantic->actual_function->declared)
            parser_semantic->actual_function->arguments_count = 0;

        parser_semantic->actual_function->declared = true;
        parser_semantic->actual_function->defined = true;
    }

    return true;
}

bool parser_semantic_set_function_return_data_type(ParserSemantic* parser_semantic, DataType data_type) {
    NULL_POINTER_CHECK(parser_semantic, false);

    if(parser_semantic->actual_action == SEMANTIC_ACTION__FUNCTION_DECLARATION ||
       parser_semantic->actual_function->return_data_type == DATA_TYPE_NONE) {

        parser_semantic->actual_function->return_data_type = data_type;

    } else if(parser_semantic->actual_action == SEMANTIC_ACTION__FUNCTION_DEFINITION) {

        if(parser_semantic->actual_function->return_data_type != data_type) {
            parser_semantic->error_report.error_code = ERROR_SEMANTIC_TYPE;
            return false;
        }
    }

    return true;
}

bool parser_semantic_add_function_parameter(ParserSemantic* parser_semantic, char* name, DataType data_type) {
    NULL_POINTER_CHECK(parser_semantic, false);

    if(parser_semantic->actual_action == SEMANTIC_ACTION__FUNCTION_DECLARATION || !parser_semantic->function_declared) {

        // Check duplicity name
        for(size_t i = 1; i <= (size_t) parser_semantic->actual_function->arguments_count; i++) {
            SymbolFunctionParam* param = symbol_function_get_param(parser_semantic->actual_function, i - 1);
            if(strcmp(param->name, name) == 0) {
                parser_semantic->error_report.error_code = ERROR_SEMANTIC_DEFINITION;
                return false;
            }

        }

        // Function declaration, add function argument
        symbol_function_add_param(parser_semantic->actual_function, name, data_type);
        parser_semantic->actual_function->arguments_count++;

    } else if(parser_semantic->actual_action == SEMANTIC_ACTION__FUNCTION_DEFINITION) {

        // Function definition, check argument on actual index
        SymbolFunctionParam* parameter = symbol_function_get_param(
                parser_semantic->actual_function,
                parser_semantic->argument_index++
        );

        if(parameter == NULL || data_type != parameter->data_type) {
            parser_semantic->error_report.error_code = ERROR_SEMANTIC_DEFINITION;
            return false;
        }
    }

    return true;
}

bool parser_semantic_check_count_of_function_arguments(ParserSemantic* parser_semantic) {
    NULL_POINTER_CHECK(parser_semantic, false);
    NULL_POINTER_CHECK(parser_semantic->actual_function, false);

    if(parser_semantic->actual_action == SEMANTIC_ACTION__FUNCTION_DEFINITION &&
       parser_semantic->actual_function->arguments_count != parser_semantic->argument_index &&
       parser_semantic->function_declared) {
        parser_semantic->error_report.error_code = ERROR_SEMANTIC_TYPE;
        return false;
    }

    return true;
}

bool parser_semantic_check_function_definitions(ParserSemantic* parser_semantic) {
    NULL_POINTER_CHECK(parser_semantic, false);

    return symbol_function_find_declared_function_without_definition(
            parser_semantic->register_->functions
    ) == NULL;
}

void parser_semantic_add_built_in_functions(ParserSemantic* parser_semantic) {
    UNUSED(parser_semantic);
    // TODO: ADD built-in function into table
}

void parser_semantic_setup_temp_variables(ParserSemantic* parser_semantic) {
    NULL_POINTER_CHECK(parser_semantic,);

    parser_semantic->temp_variable1 = symbol_table_variable_get_or_create(
            parser_semantic->register_->variables->symbol_table,
            "%__temp_variable_1"
    );
    parser_semantic->temp_variable2 = symbol_table_variable_get_or_create(
            parser_semantic->register_->variables->symbol_table,
            "%__temp_variable_2"
    );
    parser_semantic->temp_variable3 = symbol_table_variable_get_or_create(
            parser_semantic->register_->variables->symbol_table,
            "%__temp_variable_3"
    );
    parser_semantic->temp_variable1->frame =
    parser_semantic->temp_variable2->frame =
    parser_semantic->temp_variable3->frame = VARIABLE_FRAME_GLOBAL;
}

OperationSignature*
parser_semantic_get_operation_signature(ParserSemantic* parser_semantic, TypeExpressionOperation operation_type,
                                        DataType operand_1_type, DataType operand_2_type) {
    LList* operation_signatures = parser_semantic->operations_signatures[operation_type];
    OperationSignature* single_operation_signature = (OperationSignature*) operation_signatures->head;

    while(single_operation_signature != NULL) {
        if(operands_match_data_type_combination(operand_1_type, operand_2_type,
                                                single_operation_signature->operand_1_type,
                                                single_operation_signature->operand_2_type)) {
            return single_operation_signature;
        }

        single_operation_signature = (OperationSignature*) single_operation_signature->base.next;
    }

    return NULL;
}


DataType parser_semantic_resolve_implicit_data_type_conversion(ParserSemantic* parser_semantic,
                                                               TypeExpressionOperation operation_type,
                                                               DataType operand_1_type, DataType operand_2_type) {
    OperationSignature* op_signature = parser_semantic_get_operation_signature(
            parser_semantic, operation_type, operand_1_type, operand_2_type);

    if(op_signature == NULL)
        return DATA_TYPE_NONE;
    return op_signature->conversion_target_type;
}

void parser_semantic_add_operation_signature(ParserSemantic* parser_semantic, TypeExpressionOperation operation,
                                             DataType operand_1_type, DataType operand_2_type, DataType target_type,
                                             DataType result_type) {
    OperationSignature* operation_signature = (OperationSignature*) llist_new_tail_item(
            parser_semantic->operations_signatures[operation]);
    operation_signature->operation_type = operation;
    operation_signature->operand_1_type = operand_1_type;
    operation_signature->operand_2_type = operand_2_type;
    operation_signature->conversion_target_type = target_type;
    operation_signature->result_type = result_type;
}

