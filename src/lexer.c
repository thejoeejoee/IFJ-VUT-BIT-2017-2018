#include "lexer.h"
#include "memory.h"
#include "debug.h"
#include "lexer_fsm.h"


Lexer* lexer_init(lexer_input_stream_f input_stream) {
    Lexer* lexer = (Lexer*) memory_alloc(sizeof(Lexer));
    CharStack* stack = char_stack_init();

    NULL_POINTER_CHECK(input_stream, NULL);

    lexer->input_stream = input_stream;
    lexer->stack = stack;

    return lexer;
}

void lexer_free(Lexer** lexer) {
    NULL_POINTER_CHECK(lexer,);
    NULL_POINTER_CHECK(*lexer,);

    char_stack_free(&(*lexer)->stack);
    memory_free(*lexer);
    *lexer = NULL;
}

Token* lexer_next_token(Lexer* lexer) {
    NULL_POINTER_CHECK(lexer, NULL);

    // TODO: mem leak, processor should free loaded tokens
    Token* token = memory_alloc(sizeof(Token));
    NULL_POINTER_CHECK(token, NULL);

    token->type = TOKEN_UNKNOWN;

    LexerFSMState actual_state = LEX_FSM__INIT;
    do {
        // loop from init state to one of final state
        actual_state = lexer_fsm_next_state(actual_state, lexer->input_stream, lexer->stack);
    } while(!lexer_fsm_is_final_state(actual_state));

    token->type = (TokenType) actual_state;

    return token;

}