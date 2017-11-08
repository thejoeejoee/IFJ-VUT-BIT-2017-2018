#include <math.h>
#include "lexer.h"
#include "memory.h"
#include "debug.h"
#include "lexer_fsm.h"
#include "common.h"


Lexer* lexer_init(lexer_input_stream_f input_stream) {
    Lexer* lexer = (Lexer*) memory_alloc(sizeof(Lexer));
    LexerFSM* lexer_fsm = lexer_fsm_init(input_stream);

    NULL_POINTER_CHECK(input_stream, NULL);

    lexer->lexer_fsm = lexer_fsm;
    lexer->is_token_rewind = false;
    lexer->error_report.error_code = ERROR_NONE;

    return lexer;
}

void lexer_free(Lexer** lexer) {
    NULL_POINTER_CHECK(lexer,);
    NULL_POINTER_CHECK(*lexer,);

    lexer_fsm_free(&((*lexer)->lexer_fsm));
    if((*lexer)->is_token_rewind) {
        token_free(&(*lexer)->rewind_token);
    }
    memory_free(*lexer);
    *lexer = NULL;
}

void lexer_rewind_token(Lexer* lexer, Token token) {
    NULL_POINTER_CHECK(lexer,);

    ASSERT(!lexer->is_token_rewind);
    lexer->is_token_rewind = true;
    lexer->rewind_token = token_copy(token);
}

void lexer_transform_integer_value(char** integer_value) {
    NULL_POINTER_CHECK(integer_value,);
    NULL_POINTER_CHECK(*integer_value,);

    size_t target_length = (1 + strlen(*integer_value)) * 2;
    char* integer_value_copy = memory_alloc(target_length * sizeof(char));

    int result = 0;

    size_t multiplier = 0;
    size_t base;
    // First char is type of integer. [0-9] -> decimal, 'b' -> binary, 'o' -> octa, 'h' -> hexa
    switch(**integer_value) {
        case 'b':
            base = 2;
            break;

        case 'o':
            base = 8;
            break;
        case 'h':
            base = 16;
            break;

        default:
            base = 10;
            break;
    }

    for(size_t i = strlen(*integer_value) - 1; i >= 1; i--) {
        result += hex_to_int((*integer_value)[i]) * pow(base, multiplier);
        multiplier++;
    }

    snprintf(integer_value_copy, target_length, "%d", result);

    memory_free(*integer_value);
    *integer_value = integer_value_copy;

}

Token lexer_next_token(Lexer* lexer) {
    Token token = {
            .data = NULL,
            .type = TOKEN_UNKNOWN
    };

    NULL_POINTER_CHECK(lexer, token);

    if(lexer->is_token_rewind) {
        lexer->is_token_rewind = false;

        Token tmp = {.data = lexer->rewind_token.data, .type = lexer->rewind_token.type};
        lexer->rewind_token.data = NULL;
        lexer->rewind_token.type = TOKEN_UNKNOWN;

        return tmp;
    }

    LexerFSMState actual_state = LEX_FSM__INIT;
    do {
        // loop from init state to one of final state
        actual_state = lexer_fsm_next_state(lexer->lexer_fsm, actual_state);
    } while(!lexer_fsm_is_final_state(actual_state));

    token.type = (TokenType) actual_state;

    token.data = lexer_store_token_data(lexer, token);
    string_clear(lexer->lexer_fsm->stream_buffer);

    // Set error information
    if(token.type == TOKEN_ERROR) {
        lexer->error_report.error_code = ERROR_LEXER;
        lexer->error_report.detail_information = (int) lexer->lexer_fsm->lexer_error;
    }

    // Transform integer value to decimal system
    if(token.type == TOKEN_INTEGER_LITERAL) {
        // Transform size of memory
        char* copy = c_string_copy(token.data);
        memory_free(token.data);
        token.data = memory_alloc(sizeof(char) * strlen(copy) * 2);

        // Copy data, I need memory after string
        strcpy(token.data, copy);

        // transform to integer format
        lexer_transform_integer_value(&token.data);
        memory_free(copy);
    }

    return token;
}

char* lexer_store_token_data(const Lexer* lexer, Token token) {
    NULL_POINTER_CHECK(lexer, NULL);

    if(
            token.type == TOKEN_IDENTIFIER ||
            token.type == TOKEN_STRING_VALUE ||
            token.type == TOKEN_INTEGER_LITERAL ||
            token.type == TOKEN_DOUBLE_LITERAL
            ) {

        return c_string_copy(string_content(lexer->lexer_fsm->stream_buffer));
    }

    return NULL;
}
