#include <utility>

#include "gtest/gtest.h"
#include "utils/stringbycharprovider.h"


extern "C" {
#include "../src/lexer.h"
#include "../src/memory.h"
}

class LexerTokenizerTestFixture : public ::testing::Test {
    protected:
        Lexer* lexer;
        StringByCharProvider* provider;

        virtual void SetUp() {
            lexer = lexer_init(token_stream);
            provider = StringByCharProvider::instance();
        }

        virtual void TearDown() {
            lexer_free(&lexer);
        }

};


TEST_F(LexerTokenizerTestFixture, MathTokens) {
    provider->setString("+ \n -     + \t *");

    EXPECT_EQ(
<<<<<<< dca6e92d804bf0fe312d920805a615b45ed8a6d7
            lexer_next_token(lexer)->type,
            TOKEN_ADD
    ) << "Error get add token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_SUBTRACT
    ) << "Error get subtract token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_ADD
    ) << "Error get add token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_MULTIPLY
    ) << "Error get multiply token";

=======
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream, stack),
            LEX_FSM__ADD
    ) << "Math add.";
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream, stack),
            LEX_FSM__SUBTRACT
    ) << "Math subtract.";
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream, stack),
            LEX_FSM__MULTIPLY
    ) << "Math multiply.";
}

TEST_F(LexerFSMTestFixture, RelationOperators) {
    StringByCharProvider* provider = StringByCharProvider::instance();
    char_stack_empty(stack);

    provider->setString("<");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream, stack),
            LEX_FSM__LEFT_SHARP_BRACKET
    ) << "Left sharp bracker.";

    provider->setString(">");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__INIT, token_stream, stack),
            LEX_FSM__RIGHT_SHARP_BRACKET
    ) << "Right sharp brackerr.";

    provider->setString("=");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__RIGHT_SHARP_BRACKET, token_stream, stack),
            LEX_FSM__BIGGER_SAME
    ) << "Left sharp bracker.";

    provider->setString("=");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__LEFT_SHARP_BRACKET, token_stream, stack),
            LEX_FSM__SMALLER_SAME
    ) << "Right sharp brackerr.";

    provider->setString(" ");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__RIGHT_SHARP_BRACKET, token_stream, stack),
            LEX_FSM__BIGGER
    ) << "Left sharp bracker.";

    provider->setString(" ");
    EXPECT_EQ(
            lexer_fsm_next_state(LEX_FSM__LEFT_SHARP_BRACKET, token_stream, stack),
            LEX_FSM__SMALLER
    ) << "Right sharp brackerr.";

>>>>>>> Repair error in get token, add identifier token and states

}

