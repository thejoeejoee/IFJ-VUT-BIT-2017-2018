#include <utility>

#include "gtest/gtest.h"
#include "utils/stringbycharprovider.h"


extern "C" {
#include "../src/lexer.h"
#include "../src/memory.h"
}

class LexerTokenizerTestFixture : public ::testing::Test {
    protected:
        Lexer *lexer;

        virtual void SetUp() {
            memory_manager_enter(nullptr);
            lexer = lexer_init(token_stream);
        }

};



TEST_F(LexerTokenizerTestFixture, MathTokens) {
    StringByCharProvider* provider = StringByCharProvider::instance();

    provider->setString("+ \n - \t *");

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
            TOKEN_MULTIPLY
    ) << "Error get multiply token";


}

TEST_F(LexerTokenizerTestFixture, IdentifierMath) {
    StringByCharProvider* provider = StringByCharProvider::instance();

    provider->setString("aaaa + -");

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_ADD
    ) << "Error get identifier token";

}

