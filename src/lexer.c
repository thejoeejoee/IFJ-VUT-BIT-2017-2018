#include "lexer.h"
#include "memory.h"
#include "debug.h"


Lexer* lexer_init(lexer_input_stream_f input_stream) {
    Lexer* lexer = memory_alloc(sizeof(Lexer));
    NULL_POINTER_CHECK(lexer, NULL);
    NULL_POINTER_CHECK(input_stream, NULL);

    lexer->input_stream = input_stream;

    return lexer;
}

Token* lexer_next_token(Lexer* lexer) {
    NULL_POINTER_CHECK(lexer, NULL);

    Token* token = memory_alloc(sizeof(Token));
    token->type = TOKEN_UNKNOWN;

    int c = lexer->input_stream();

    return token;

}