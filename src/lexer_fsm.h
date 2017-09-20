#ifndef _LEXER_FSM_H
#define _LEXER_FSM_H

typedef int (* lexer_input_stream_f)();

typedef enum {
    LEX_FSM__INIT,
    LEX_FSM__COMMENT_LINE,
    LEX_FSM__COMMENT_BLOCK,

    LEX_FSM__DIVIDE,


    LEX_FSM__LEG_SHOT
} LexerFSMState;

LexerFSMState lexer_fsm_next_state(LexerFSMState prev_state, lexer_input_stream_f input_stream);

#endif // _LEXER_FSM_H