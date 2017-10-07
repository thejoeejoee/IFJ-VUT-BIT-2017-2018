#include "parser_semantic.h"


ParserSemantic* parser_semantic_init() {
    ParserSemantic* parser_semantic = memory_alloc(sizeof(ParserSemantic));

    parser_semantic->register_ = symbol_register_init();
    parser_semantic->actual_action = SEMANTIC_ACTION__NONE;
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

bool parser_semantic_add_symbol_variable(ParserSemantic* parser_semantic, char* name, DataType data_type) {
    NULL_POINTER_CHECK(parser_semantic, false);
    NULL_POINTER_CHECK(name, false);
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
    return true;
}

bool parser_semantic_set_function_name(ParserSemantic* parser_semantic, char* name) {
    NULL_POINTER_CHECK(parser_semantic, false);
    NULL_POINTER_CHECK(name, false);

    if(parser_semantic->actual_action == SEMANTIC_ACTION__FUNCTION_DECLARATION) {
        if(symbol_table_function_get(parser_semantic->register_->functions, name) != NULL) {
            parser_semantic->error_report.error_code = ERROR_SEMANTIC_DEFINITION;
            return false;
        }

        parser_semantic->symbol_function = symbol_table_function_get_or_create(
                parser_semantic->register_->functions,
                name
        );

        parser_semantic->symbol_function->declared = true;
    } else if(parser_semantic->actual_action == SEMANTIC_ACTION__FUNCTION_DEFINITION) {
        parser_semantic->symbol_function = symbol_table_function_get_or_create(
                parser_semantic->register_->functions,
                name
        );

        if(parser_semantic->symbol_function->defined)
            return false;

        parser_semantic->symbol_function->defined = true;
    }

    return true;
}

bool parser_semantic_set_function_return_data_type(ParserSemantic* parser_semantic, DataType data_type) {
    NULL_POINTER_CHECK(parser_semantic, false);

    if(parser_semantic->actual_action == SEMANTIC_ACTION__FUNCTION_DECLARATION ||
       parser_semantic->symbol_function->return_data_type == DATA_TYPE_NONE) {

        parser_semantic->symbol_function->return_data_type = data_type;

    } else if(parser_semantic->actual_action == SEMANTIC_ACTION__FUNCTION_DEFINITION) {
        if(parser_semantic->symbol_function->return_data_type != data_type)
            return false;
    }

    return true;
}
