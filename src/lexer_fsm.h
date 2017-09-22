#ifndef _LEXER_FSM_H
#define _LEXER_FSM_H

#include <stdbool.h>
#include "char_stack.h"

#define LEXER_FSM_STACK_SIZE 128

typedef int (* lexer_input_stream_f)();

typedef enum {

    // Start state
    LEX_FSM__INIT,

    // Comments
    LEX_FSM__COMMENT_LINE,
    LEX_FSM__SLASH,
    LEX_FSM__COMMENT_BLOCK,
    LEX_FSM__COMMENT_BLOCK_END,

    // Operations in mathematical expressions
    LEX_FSM__ADD,
    LEX_FSM__SUBTRACT,
    LEX_FSM__DIVIDE,
    LEX_FSM__MULTIPLY,

    // Brackets
    LEX_FSM__LEFT_BRACKET,
    LEX_FSM__RIGHT_BRACKET,
    LEX_FSM__LEFT_SHARP_BRACKET,
    LEX_FSM__RIGHT_SHARP_BRACKET,

    // Comparatoirs
    LEX_FSM__SMALLER,
    LEX_FSM__BIGGER,
    LEX_FSM__SMALLER_SAME,
    LEX_FSM__BIGGER_SAME,

    // Identifier states
    LEX_FSM__IDENTIFIER_UNFINISHED,
    LEX_FSM__IDENTIFIER_FINISHED,

    // Error state
    LEX_FSM__LEG_SHOT

} LexerFSMState;

/**
 * @brief Get next state from prev state and next symbol
 *
 * @param LexerFSMState prev_state
 * @param lexer_input_stream_f input_stream
 * @return LexerFSMState
 */
LexerFSMState lexer_fsm_next_state(LexerFSMState prev_state, lexer_input_stream_f input_stream, CharStack* stack);

/**
 * @brief Find out if the state is final
 *
 * @param LexerFSMState state actual state
 * @return bool
 */
bool lexer_fsm_is_final_state(LexerFSMState state);

/**
 * @brief Find out if the character is white space
 *
 * @param char c
 * @return bool
 */
bool lexer_is_white_space(char c);

#endif // _LEXER_FSM_H