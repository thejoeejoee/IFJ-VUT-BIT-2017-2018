#include "lexer.h"
#include "memory.h"
#include "debug.h"
#include "lexer_fsm.h"


Lexer* lexer_init(lexer_input_stream_f input_stream) {
    Lexer* lexer = (Lexer *) memory_alloc(sizeof(Lexer));

    NULL_POINTER_CHECK(input_stream, NULL);

    lexer->input_stream = input_stream;

    return lexer;
}

Token* lexer_next_token(Lexer* lexer) {
    NULL_POINTER_CHECK(lexer, NULL);

    Token* token = memory_alloc(sizeof(Token));
    token->type = TOKEN_UNKNOWN;

    LexerFSMState actual_state;
    do {
        actual_state = lexer_fsm_next_state(LEX_FSM__INIT, lexer->input_stream);
    } while (!is_final_state(actual_state));

    token->type = (TokenType) actual_state;

    return token;

}