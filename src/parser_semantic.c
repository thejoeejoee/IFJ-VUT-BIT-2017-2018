#include "parser_semantic.h"


ParserSemantic* parser_semantic_init() {
    ParserSemantic* parser_semantic = memory_alloc(sizeof(ParserSemantic));

    parser_semantic->register_ = symbol_register_init();
    return parser_semantic;
}

void parser_semantic_free(ParserSemantic** parser) {
    NULL_POINTER_CHECK(parser,);
    NULL_POINTER_CHECK(*parser,);

    symbol_register_free(&(*parser)->register_);
    memory_free(*parser);
    *parser = NULL;
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
    if(symbol_register_find_variable(parser_semantic->register_, name) != NULL)
        return false;

    SymbolVariable* symbol_variable = symbol_table_variable_get_or_create(
            parser_semantic->register_->variables->symbol_table,
            name
    );

    symbol_variable->data_type = data_type;
    return true;
}
