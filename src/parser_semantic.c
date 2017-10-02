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
