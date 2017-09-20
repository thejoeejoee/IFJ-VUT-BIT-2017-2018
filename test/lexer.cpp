#include "gtest/gtest.h"

#include "../src/lexer_fsm.c"

TEST(LexerFSM, LineComment) {
    EXPECT_EQ(
            lexer_fsm_next_state('@', LEX_FSM__INIT),
            LEX_FSM__LEG_SHOT
    );
    EXPECT_EQ(
            lexer_fsm_next_state('\'', LEX_FSM__INIT),
            LEX_FSM__COMMENT_LINE
    );
    EXPECT_EQ(
            lexer_fsm_next_state('a', LEX_FSM__COMMENT_LINE),
            LEX_FSM__COMMENT_LINE
    );
    EXPECT_EQ(
            lexer_fsm_next_state('\'', LEX_FSM__COMMENT_LINE),
            LEX_FSM__COMMENT_LINE
    );
    EXPECT_EQ(
            lexer_fsm_next_state('\n', LEX_FSM__COMMENT_LINE),
            LEX_FSM__INIT
    );
}