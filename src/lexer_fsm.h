#ifndef _LEXER_FSM_H
#define _LEXER_FSM_H

#include <stdbool.h>

typedef int (* lexer_input_stream_f)();

typedef enum {

    // Start state
    LEX_FSM__INIT,

    // Comments
    LEX_FSM__COMMENT_LINE,
    LEX_FSM__SLASH,
    LEX_FSM_COMMENT_BLOCK,
    LEX_FSM_COMMENT_BLOCK_END,

    // Operations in mathematical expressions
    LEX_FSM__ADD,
    LEX_FSM__SUBTRACT,
    LEX_FSM__DIVIDE,
    LEX_FSM__MULTIPLY,

    // Brackets
    LEX_FSM__LEFT_BRACKET,
    LEX_FSM__RIGHT_BRACKET,


    LEX_FSM__LEG_SHOT

} LexerFSMState;

/**
 * @brief Get next state from prev state and next symbol
 *
 * @param LexerFSMState prev_state
 * @param lexer_input_stream_f input_stream
 * @return LexerFSMState
 */
LexerFSMState lexer_fsm_next_state(LexerFSMState prev_state, lexer_input_stream_f input_stream);

/**
 * @brief Find out if the state is final
 *
 * @param LexerFSMState state actual state
 * @return bool
 */
bool is_final_state(LexerFSMState state);

#endif // _LEXER_FSM_H