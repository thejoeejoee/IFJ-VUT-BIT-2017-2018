#ifndef _LEXER_FSM_H
#define _LEXER_FSM_H

typedef enum {
    LEX_FSM__INIT,
    LEX_FSM__COMMENT_LINE,


    LEX_FSM__LEG_SHOT
} LexerFSMState;

#endif // _LEXER_FSM_H