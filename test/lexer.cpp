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


}

TEST_F(LexerTokenizerTestFixture, Identifiers) {
    provider->setString("ahoj _9h7___ a_9");

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER add token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER subtract token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER  add token";


}

