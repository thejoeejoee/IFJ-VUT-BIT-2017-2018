#include "lexer_fsm.h"


LexerFSMState lexer_fsm_next_state(int c, LexerFSMState prev_state) {
    switch (prev_state) {
        case LEX_FSM__INIT:
            switch (c) {
                case '\'':
                    return LEX_FSM__COMMENT_LINE;
                default:
                    break;
            }
            break;
        case LEX_FSM__COMMENT_LINE:
            if (c != '\n')
                return LEX_FSM__COMMENT_LINE;
            return LEX_FSM__INIT;

        default:
            break;
    }

    // TODO never happened, Chuck Norris state
    return LEX_FSM__LEG_SHOT;
}