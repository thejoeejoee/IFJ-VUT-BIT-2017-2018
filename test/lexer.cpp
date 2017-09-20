#include <utility>

#include "gtest/gtest.h"
#include "utils/stringbycharprovider.h"

extern "C" {
#include "../src/lexer_fsm.h"
}


int token_stream() {
    StringByCharProvider* instance = StringByCharProvider::instance();
    return instance->next_char();
}

TEST(LexerFSMTest, LineComment) {
    StringByCharProvider* provider = StringByCharProvider::instance();

    provider->set_string("@");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream),
            LEX_FSM__LEG_SHOT
    );

    provider->set_string("'");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream),
            LEX_FSM__COMMENT_LINE
    );

    provider->set_string("a");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_LINE, token_stream),
            LEX_FSM__COMMENT_LINE
    );

    provider->set_string("'");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_LINE, token_stream),
            LEX_FSM__COMMENT_LINE
    );

    provider->set_string("\n");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_LINE, token_stream),
            LEX_FSM__INIT
    );
}

TEST(LexerFSMTest, BlockComment) {
    // TODO: missing tests for block comment
}