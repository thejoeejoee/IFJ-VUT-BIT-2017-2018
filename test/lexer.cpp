#include <utility>

#include "gtest/gtest.h"
#include "utils/stringbycharprovider.h"

extern "C" {
#include "../src/lexer_fsm.h"
}


TEST(LexerFSMTest, UnknownCharacter) {
    StringByCharProvider* provider = StringByCharProvider::instance();

    provider->setString("@");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream),
            LEX_FSM__LEG_SHOT
    ) << "Unknown character for lexer.";

}

TEST(LexerFSMTest, LineComment) {
    StringByCharProvider* provider = StringByCharProvider::instance();

    provider->setString("'");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream),
            LEX_FSM__COMMENT_LINE
    ) << "Quote select line comment from init state.";

    provider->setString("a");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_LINE, token_stream),
            LEX_FSM__COMMENT_LINE
    ) << "All content is ignored in comment line.";

    provider->setString("'");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_LINE, token_stream),
            LEX_FSM__COMMENT_LINE
    ) << "Also quote is ignored in line comment.";

    provider->setString("\n");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_LINE, token_stream),
            LEX_FSM__INIT
    ) << "End of line resets line comment to init state.";
}

TEST(LexerFSMTest, BlockComment) {
    StringByCharProvider* provider = StringByCharProvider::instance();

    provider->setString("/'");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream),
            LEX_FSM__SLASH
    ) << "Slash is start character for comment or dividing.";
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__SLASH, token_stream),
            LEX_FSM__COMMENT_BLOCK
    ) << "Quote in slash state turns state into block comment.";

    provider->setString("f/");
    for (int i = 0; i < 2; ++i) {
        EXPECT_EQ(
                lexer_fsm_next_state(LEX_FSM__COMMENT_BLOCK, token_stream),
                LEX_FSM__COMMENT_BLOCK
        ) << "All in block comment is ignored.";
    }
    provider->setString("'X'/");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_BLOCK, token_stream),
            LEX_FSM__COMMENT_BLOCK_END
    ) << "Prepare for end of comment.";
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_BLOCK_END, token_stream),
            LEX_FSM__COMMENT_BLOCK
    ) << "Prepared for end of comment, but broke.";
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_BLOCK, token_stream),
            LEX_FSM__COMMENT_BLOCK_END
    ) << "Prepared for end of comment.";
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_BLOCK_END, token_stream),
            LEX_FSM__INIT
    ) << "End of comment.";
}

TEST(LexerFSMTest, MathematicOperations) {
    StringByCharProvider* provider = StringByCharProvider::instance();

    provider->setString("+-*/");

    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream),
            LEX_FSM__ADD
    ) << "Math add.";
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream),
            LEX_FSM__SUBTRACT
    ) << "Math subtract.";
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream),
            LEX_FSM__MULTIPLY
    ) << "Math multiply.";
    EXPECT_EQ(
    // TODO: ugly dependency on internal implementation...
            lexer_fsm_next_state(lexer_fsm_next_state(LEX_FSM__INIT, token_stream), token_stream),
            LEX_FSM__DIVIDE
    ) << "Math divide.";
}

TEST(LexerFSMTest, Identifier) {
    StringByCharProvider* provider = StringByCharProvider::instance();

    provider->setString("a");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream),
            LEX_FSM__IDENTIFIER_UNFINISHED
    ) << "Quote select line comment from init state.";

    provider->setString("a");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__IDENTIFIER_UNFINISHED, token_stream),
            LEX_FSM__IDENTIFIER_UNFINISHED
    ) << "Quote select line comment from init state.";

    provider->setString("a");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__IDENTIFIER_UNFINISHED, token_stream),
            LEX_FSM__IDENTIFIER_UNFINISHED
    ) << "Quote select line comment from init state.";

    provider->setString(" ");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__IDENTIFIER_UNFINISHED, token_stream),
            LEX_FSM__IDENTIFIER_FINISHED
    ) << "Quote select line comment from init state.";

}


