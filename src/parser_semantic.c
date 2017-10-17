#include "parser_semantic.h"


ParserSemantic* parser_semantic_init() {
    ParserSemantic* parser_semantic = memory_alloc(sizeof(ParserSemantic));

    parser_semantic->register_ = symbol_register_init();
    parser_semantic->actual_action = SEMANTIC_ACTION__NONE;
    parser_semantic->actual_variable = NULL;
    parser_semantic->actual_function = NULL;
    parser_semantic->temp_variable = NULL;
    return parser_semantic;
}

void parser_semantic_free(ParserSemantic** parser) {
    NULL_POINTER_CHECK(parser,);
    NULL_POINTER_CHECK(*parser,);

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

    SymbolVariable* symbol_variable = symbol_table_variable_get_or_create(
            parser_semantic->register_->variables->symbol_table,
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

        parser_semantic->actual_function->declared  = true;
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
        for(int i = 1; i <= (int)parser_semantic->actual_function->arguments_count; i++) {
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

bool parser_semantic_check_definitions(ParserSemantic* parser_semantic) {
    NULL_POINTER_CHECK(parser_semantic, false);

    SymbolFunction* symbol_function = NULL;

    symbol_function = symbol_function_find_declared_function_without_definition(parser_semantic->register_->functions);

    if(symbol_function != NULL)
        return false;

    return true;
}

