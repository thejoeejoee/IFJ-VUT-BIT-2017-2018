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
        Token *token;

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

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_ADD
    ) << "Error get add token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_SUBTRACT
    ) << "Error get subtract token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_ADD
    ) << "Error get add token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_MULTIPLY
    ) << "Error get multiply token";
    memory_free(token);
    token = NULL;


}

TEST_F(LexerTokenizerTestFixture, Identifiers) {
    provider->setString("ahoj _9h7___ a_9");

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER add token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER subtract token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER  add token";
    memory_free(token);
    token = NULL;


}

TEST_F(LexerTokenizerTestFixture, RelationOperators) {
    provider->setString("< > <= >=");
    char_stack_empty(lexer->lexer_fsm->stack);

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_SMALLER
    ) << "Error SMALLER add token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_BIGGER
    ) << "Error BIGGER subtract token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_SMALLER_EQUAL
    ) << "Error SMALLER_EQUAL token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_BIGGER_EQUAL
    ) << "Error BIGGER_EQUAL token";
    memory_free(token);
    token = NULL;


}

TEST_F(LexerTokenizerTestFixture, ComplexTest) {
    provider->setString("+ <= >= ahoj _8wtf *");
    char_stack_empty(lexer->lexer_fsm->stack);

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_ADD
    ) << "Error SMALLER add token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_SMALLER_EQUAL
    ) << "Error SMALLER_EQUAL token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_BIGGER_EQUAL
    ) << "Error BIGGER_EQUAL token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_IDENTIFIER
    ) << "Error MULTIPLY token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_MULTIPLY
    ) << "Error MULTIPLY token";
    memory_free(token);
    token = NULL;


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

    std::vector<TokenType> expectedTokens = {
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

    for (const TokenType expectedToken: expectedTokens) {
        token = lexer_next_token(lexer);
        EXPECT_EQ(
                token->type,
                expectedToken
        ) << "Error token in complex test";
        memory_free(token);
        token = NULL;
    }


}

TEST_F(LexerTokenizerTestFixture, Keywords) {
    provider->setString("AS + sCOpE");
    char_stack_empty(lexer->lexer_fsm->stack);

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_AS
    ) << "Error AS token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_ADD
    ) << "Error ADD token";
    memory_free(token);
    token = NULL;

    token = lexer_next_token(lexer);
    EXPECT_EQ(
            token->type,
            TOKEN_SCOPE
    ) << "Error SCOPE token";
    memory_free(token);
    token = NULL;


}
