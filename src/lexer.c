#include "lexer.h"
#include "memory.h"
#include "debug.h"
#include "lexer_fsm.h"


Lexer* lexer_init(lexer_input_stream_f input_stream) {
    Lexer* lexer = (Lexer*) memory_alloc(sizeof(Lexer));
    LexerFSM* lexer_fsm = lexer_fsm_init(input_stream);

    NULL_POINTER_CHECK(input_stream, NULL);

    lexer->lexer_fsm = lexer_fsm;
    lexer->is_token_rewind = false;

    return lexer;
}

void lexer_free(Lexer** lexer) {
    NULL_POINTER_CHECK(lexer,);
    NULL_POINTER_CHECK(*lexer,);

    lexer_fsm_free(&((*lexer)->lexer_fsm));
    memory_free(*lexer);
    *lexer = NULL;
}

void lexer_rewind_token(Lexer* lexer, Token token) {
    NULL_POINTER_CHECK(lexer,);

    ASSERT(!lexer->is_token_rewind);
    lexer->is_token_rewind = true;
    lexer->rewind_token = token;
}

Token lexer_next_token(Lexer* lexer) {
    Token token = {
            .data = NULL,
            .type = TOKEN_UNKNOWN
    };

    NULL_POINTER_CHECK(lexer, token);

    if(lexer->is_token_rewind) {
        lexer->is_token_rewind = false;
        return lexer->rewind_token;
    }

    LexerFSMState actual_state = LEX_FSM__INIT;
    do {
        // loop from init state to one of final state
        actual_state = lexer_fsm_next_state(lexer->lexer_fsm, actual_state);
    } while(!lexer_fsm_is_final_state(actual_state));

    token.type = (TokenType) actual_state;

    size_t data_length = string_length(lexer->lexer_fsm->stream_buffer);
    if(data_length > 0) {
        token.data = memory_alloc(sizeof(char) * (data_length + 1));

        if(NULL == strcpy(token.data, string_content(lexer->lexer_fsm->stream_buffer))) {
            exit_with_code(ERROR_INTERNAL);
        }
    }

    string_clear(lexer->lexer_fsm->stream_buffer);

    return token;

}