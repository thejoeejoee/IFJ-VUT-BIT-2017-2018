#include "parser_semantic.h"


ParserSemantic* parser_semantic_init() {
    ParserSemantic* parser_semantic = memory_alloc(sizeof(ParserSemantic));

    parser_semantic->register_ = symbol_register_init();
    parser_semantic->actual_action = ACTUAL_ACTION__NO_ACTION;
    return parser_semantic;
}

void parser_semantic_free(ParserSemantic** parser) {
    NULL_POINTER_CHECK(parser,);
    NULL_POINTER_CHECK(*parser,);

    symbol_register_free(&(*parser)->register_);
    memory_free(*parser);
    *parser = NULL;
}

void parser_semantic_set_action(ParserSemantic* parser_semantic, ActualAction actual_action) {
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

bool parser_semantic_add_symbol_variable(ParserSemantic* parser_semantic, char* name, short data_type) {
    if(symbol_register_find_variable(parser_semantic->register_, name) != NULL) {
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

bool parser_semantic_set_function_name(ParserSemantic* parserSemantic, char* name) {

    if(parserSemantic->actual_action == ACTUAL_ACTION__FUNCTION_DECLARATION) {

        if(symbol_table_function_get(parserSemantic->register_->functions, name) != NULL) {
            parserSemantic->error_report.error_code = ERROR_SEMANTIC_DEFINITION;
            return false;
        }

        parserSemantic->symbol_function = symbol_table_function_get_or_create(
                parserSemantic->register_->functions,
                name
        );

        parserSemantic->symbol_function->declared = true;

    }
    else if(parserSemantic->actual_action == ACTUAL_ACTION__FUNCTION_DEFINITION) {

        parserSemantic->symbol_function = symbol_table_function_get_or_create(
                parserSemantic->register_->functions,
                name
        );

        if(parserSemantic->symbol_function->defined)
            return false;

        parserSemantic->symbol_function->defined = true;

    }


    return true;
}

bool parser_semantic_function_return_data_type(ParserSemantic* parser_semantic, TokenType token_type) {

    if(parser_semantic->actual_action == ACTUAL_ACTION__FUNCTION_DECLARATION ||
            parser_semantic->symbol_function->return_data_type == (ActualAction) 0)

        parser_semantic->symbol_function->return_data_type = token_type;

    else if(parser_semantic->actual_action == ACTUAL_ACTION__FUNCTION_DEFINITION) {

        if(parser_semantic->symbol_function->return_data_type != token_type)
            return false;
    }

    return true;
}
