#include <utility>

#include "gtest/gtest.h"
#include "utils/stringbycharprovider.h"

extern "C" {
#include "../src/lexer_fsm.h"
}

class LexerFSMTestFixture : public ::testing::Test {
    protected:
        LexerFSM* lexer_fsm;
        StringByCharProvider* provider = StringByCharProvider::instance();

        virtual void SetUp() {
            lexer_fsm = lexer_fsm_init(token_stream);
            provider->reset();
        }

        virtual void TearDown() {
            lexer_fsm_free(&lexer_fsm);
        }

};


TEST_F(LexerFSMTestFixture, UnknownCharacter) {
    char_stack_empty(lexer_fsm->stack);

    provider->setString("@");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, lexer_fsm),
            LEX_FSM__LEG_SHOT
    ) << "Unknown character for lexer.";

}

TEST_F(LexerFSMTestFixture, LineComment) {
    provider->setString("'");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, lexer_fsm),
            LEX_FSM__COMMENT_LINE
    ) << "Quote select line comment from init state.";

    provider->setString("a");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_LINE, lexer_fsm),
            LEX_FSM__COMMENT_LINE
    ) << "All content is ignored in comment line.";

    provider->setString("'");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_LINE, lexer_fsm),
            LEX_FSM__COMMENT_LINE
    ) << "Also quote is ignored in line comment.";

    provider->setString("\n");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_LINE, lexer_fsm),
            LEX_FSM__INIT
    ) << "End of line resets line comment to init state.";
}

TEST_F(LexerFSMTestFixture, BlockComment) {
    provider->setString("/'");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, lexer_fsm),
            LEX_FSM__SLASH
    ) << "Slash is start character for comment or dividing.";
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__SLASH, lexer_fsm),
            LEX_FSM__COMMENT_BLOCK
    ) << "Quote in slash state turns state into block comment.";

    provider->setString("f/");
    for(int i = 0; i < 2; ++i) {
        EXPECT_EQ(
                lexer_fsm_next_state(LEX_FSM__COMMENT_BLOCK, lexer_fsm),
                LEX_FSM__COMMENT_BLOCK
        ) << "All in block comment is ignored.";
    }
    provider->setString("'X'/");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_BLOCK, lexer_fsm),
            LEX_FSM__COMMENT_BLOCK_END
    ) << "Prepare for end of comment.";
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_BLOCK_END, lexer_fsm),
            LEX_FSM__COMMENT_BLOCK
    ) << "Prepared for end of comment, but broke.";
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_BLOCK, lexer_fsm),
            LEX_FSM__COMMENT_BLOCK_END
    ) << "Prepared for end of comment.";
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__COMMENT_BLOCK_END, lexer_fsm),
            LEX_FSM__INIT
    ) << "End of comment.";
}

TEST_F(LexerFSMTestFixture, MathematicOperations) {
    provider->setString("+-*/");

    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, lexer_fsm),
            LEX_FSM__ADD
    ) << "Math add.";
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, lexer_fsm),
            LEX_FSM__SUBTRACT
    ) << "Math subtract.";
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, lexer_fsm),
            LEX_FSM__MULTIPLY
    ) << "Math multiply.";
}

TEST_F(LexerFSMTestFixture, Identifier) {
    provider->setString("a");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, lexer_fsm),
            LEX_FSM__IDENTIFIER_UNFINISHED
    ) << "Quote select line comment from init state.";

    provider->setString("a");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__IDENTIFIER_UNFINISHED, lexer_fsm),
            LEX_FSM__IDENTIFIER_UNFINISHED
    ) << "Quote select line comment from init state.";

    provider->setString("a");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__IDENTIFIER_UNFINISHED, lexer_fsm),
            LEX_FSM__IDENTIFIER_UNFINISHED
    ) << "Quote select line comment from init state.";

    provider->setString(" ");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__IDENTIFIER_UNFINISHED, lexer_fsm),
            LEX_FSM__IDENTIFIER_FINISHED
    ) << "Quote select line comment from init state.";

}


