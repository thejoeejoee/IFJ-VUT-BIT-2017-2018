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

TEST_F(LexerTokenizerTestFixture, RelationOperators) {
    provider->setString("< > <= >=");
    char_stack_empty(lexer->lexer_fsm->stack);

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_SMALLER
    ) << "Error SMALLER add token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_BIGGER
    ) << "Error BIGGER subtract token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_SMALLER_EQUAL
    ) << "Error SMALLER_EQUAL token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_BIGGER_EQUAL
    ) << "Error BIGGER_EQUAL token";


}

TEST_F(LexerTokenizerTestFixture, ComplexTest) {
    provider->setString("+ <= >= ahoj _8wtf *");
    char_stack_empty(lexer->lexer_fsm->stack);

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_ADD
    ) << "Error SMALLER add token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_SMALLER_EQUAL
    ) << "Error SMALLER_EQUAL token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_BIGGER_EQUAL
    ) << "Error BIGGER_EQUAL token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_IDENTIFIER
    ) << "Error MULTIPLY token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_MULTIPLY
    ) << "Error MULTIPLY token";


}

TEST_F(LexerTokenizerTestFixture, SecondComplexTest) {
    provider->setString("/' Program 2: Vypocet faktorialu (rekurzivne) '/\n"
                                "Declare Function factorial (n As Integer) As Integer\n"
                                "Function factorial (n As Integer) As Integer\n"
                                "Dim temp_result As Integer\n"
                                "Dim decremented_n As Integer\n"
                                "Dim result As Integer\n"
                                "If n < 2 Then\n"
                                "result = 1\n"
                                "Else\n"
                                "decremented_n = n - 1\n"
                                "temp_result = factorial(decremented_n)\n"
                                "result = n * temp_result\n"
                                "End If\n"
                                "Return result\n"
                                "End Function");
    char_stack_empty(lexer->lexer_fsm->stack);

    const int count_of_tokens = 27;

    TokenType expected_tokens[] = {
            TOKEN_DECLARE, TOKEN_FUNCTION, TOKEN_IDENTIFIER,
            TOKEN_LEFT_BRACKET, TOKEN_IDENTIFIER, TOKEN_AS,
            TOKEN_INTEGER, TOKEN_RIGHT_BRACKET, TOKEN_AS,
            TOKEN_INTEGER, TOKEN_FUNCTION, TOKEN_IDENTIFIER,
            TOKEN_LEFT_BRACKET, TOKEN_IDENTIFIER, TOKEN_AS,
            TOKEN_INTEGER, TOKEN_RIGHT_BRACKET, TOKEN_AS,
            TOKEN_INTEGER, TOKEN_DIM, TOKEN_IDENTIFIER,
            TOKEN_AS, TOKEN_INTEGER, TOKEN_DIM,
            TOKEN_IDENTIFIER, TOKEN_AS, TOKEN_INTEGER
    };

    for (int i = 0; i < count_of_tokens; i++) {
        EXPECT_EQ(
                lexer_next_token(lexer)->type,
                expected_tokens[i]
        ) << "Error token in complex test";
    }


}

TEST_F(LexerTokenizerTestFixture, Keywords) {
    provider->setString("AS + sCOpE");
    char_stack_empty(lexer->lexer_fsm->stack);

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_AS
    ) << "Error AS token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_ADD
    ) << "Error ADD token";

    EXPECT_EQ(
            lexer_next_token(lexer)->type,
            TOKEN_SCOPE
    ) << "Error SCOPE token";


}
