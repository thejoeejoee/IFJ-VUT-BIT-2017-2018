#include "lexer_fsm.h"


LexerFSMState lexer_fsm_next_state(int c, LexerFSMState prev_state) {
    switch (prev_state) {
        case LexerFSMState::INIT:
            switch (c) {
                case '\'':
                    return LexerFSMState::COMMENT__LINE;
                default:
                    break;
            }
        case COMMENT__LINE:
            if (c != '\n')
                return COMMENT__LINE;
            return INIT;

        default:
            break;
    }

    // TODO never happened, Chuck Norris state
    return LexerFSMState::INIT;
}