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

        void SetUp() override {
            lexer = lexer_init(token_stream);
            provider = StringByCharProvider::instance();
        }

        void TearDown() override {
            lexer_free(&lexer);
        }

        TokenType getNextTokenType() {
            Token token = lexer_next_token(lexer);
            const TokenType tokenType = token.type;
            token_free(&token);
            return tokenType;
        }
};

TEST_F(LexerTokenizerTestFixture, Keywords) {
    provider->setString("AS + sCOpE");
    char_stack_empty(lexer->lexer_fsm->stack);

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_AS
    ) << "Error AS token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_ADD
    ) << "Error ADD token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_SCOPE
    ) << "Error SCOPE token";
}

TEST_F(LexerTokenizerTestFixture, MathTokens) {
    provider->setString("+ \n -     + \t * /");

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_ADD
    ) << "Error get add token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_EOL
    ) << "Error get add token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_SUBTRACT
    ) << "Error get subtract token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_ADD
    ) << "Error get add token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_MULTIPLY
    ) << "Error get multiply token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_DIVIDE
    ) << "Error get divide token";
}

TEST_F(LexerTokenizerTestFixture, Strings) {
    provider->setString(R"RAW(
!"Simple string"
!"\\\n _hudkghj6878"
!"\\\t"
!"Chuck norris \\\" \\\""
!"Chuck \\123 norris \\\" \\\""
!"Chuck \\056 norris \\\" \\\""
!"Chuck \\056 \\222 norris \\\" \\\""
)RAW");
    char_stack_empty(lexer->lexer_fsm->stack);

    const std::vector<TokenType> expectedTokens = {
            TOKEN_EOL,
            TOKEN_STRING_VALUE,
            TOKEN_EOL,
            TOKEN_STRING_VALUE,
            TOKEN_EOL,
            TOKEN_STRING_VALUE,
            TOKEN_EOL,
            TOKEN_STRING_VALUE,
            TOKEN_EOL,
            TOKEN_STRING_VALUE,
            TOKEN_EOL,
            TOKEN_STRING_VALUE,
            TOKEN_EOL,
            TOKEN_STRING_VALUE,
            TOKEN_EOL,
            TOKEN_EOF
    };

    for(const TokenType expectedToken: expectedTokens) {
        EXPECT_EQ(
                this->getNextTokenType(),
                expectedToken
        ) << "Error string token";
    }


}

TEST_F(LexerTokenizerTestFixture, IntegersAndDoubles) {
    provider->setString(R"RAW(
124667257
1221342.54654
234346.4646e465
1221342.54654e-245
1221342.54654e+245
234346e465
1221342e-245
1221342e+245
)RAW");
    char_stack_empty(lexer->lexer_fsm->stack);

    const std::vector<TokenType> expectedTokens = {
            TOKEN_EOL,
            TOKEN_INTEGER_LITERAL,
            TOKEN_EOL,
            TOKEN_DOUBLE_LITERAL,
            TOKEN_EOL,
            TOKEN_DOUBLE_LITERAL,
            TOKEN_EOL,
            TOKEN_DOUBLE_LITERAL,
            TOKEN_EOL,
            TOKEN_DOUBLE_LITERAL,
            TOKEN_EOL,
            TOKEN_DOUBLE_LITERAL,
            TOKEN_EOL,
            TOKEN_DOUBLE_LITERAL,
            TOKEN_EOL,
            TOKEN_DOUBLE_LITERAL,
            TOKEN_EOL,
            TOKEN_EOF

    };

    for(const TokenType expectedToken: expectedTokens) {
        EXPECT_EQ(
                this->getNextTokenType(),
                expectedToken
        );
    }
}

TEST_F(LexerTokenizerTestFixture, Identifiers) {
    provider->setString("ahoj _9h7___ a_9");

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER add token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER subtract token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER  add token";
}

TEST_F(LexerTokenizerTestFixture, EOFToken) {
    provider->setString("");

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_EOF
    ) << "Error EOF token";

    provider->setString("ahoj ");

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_EOF
    ) << "Error EOF token";
}

TEST_F(LexerTokenizerTestFixture, RelationOperators) {
    provider->setString("< > <= >= <>");
    char_stack_empty(lexer->lexer_fsm->stack);

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_SMALLER
    ) << "Error SMALLER add token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_BIGGER
    ) << "Error BIGGER subtract token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_SMALLER_EQUAL
    ) << "Error SMALLER_EQUAL token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_BIGGER_EQUAL
    ) << "Error BIGGER_EQUAL token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_SMALLER_BIGGER
    ) << "Error SMALLER_BIGGER token";
}

TEST_F(LexerTokenizerTestFixture, ErrorTokens) {

    provider->setString("@");
    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_ERROR
    ) << "Error ERROR token";

    EXPECT_EQ(
            lexer->lexer_fsm->lexer_error,
            LEXER_ERROR__ERROR_LEXEM
    ) << "Error getting error code";

    provider->setString("AHOJ  @");
    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_ERROR
    ) << "Error ERROR token";

    EXPECT_EQ(
            lexer->lexer_fsm->lexer_error,
            LEXER_ERROR__ERROR_LEXEM
    ) << "Error getting error code";

    provider->setString(R"(!"\%")");
    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_ERROR
    ) << "Error ERROR token";

    EXPECT_EQ(
            lexer->lexer_fsm->lexer_error,
            LEXER_ERROR__STRING_FORMAT
    ) << "Error getting error code";

    provider->setString("!\"\n\"");
    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_ERROR
    ) << "Error ERROR token";

    EXPECT_EQ(
            lexer->lexer_fsm->lexer_error,
            LEXER_ERROR__STRING_FORMAT
    ) << "Error getting error code";

    provider->setString("123.");
    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_ERROR
    ) << "Error ERROR token";

    EXPECT_EQ(
            lexer->lexer_fsm->lexer_error,
            LEXER_ERROR__DOUBLE_FORMAT
    ) << "Error getting error code";

    provider->setString("127.12321edf");
    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_ERROR
    ) << "Error ERROR token";

    EXPECT_EQ(
            lexer->lexer_fsm->lexer_error,
            LEXER_ERROR__DOUBLE_FORMAT
    ) << "Error getting error code";

    provider->setString("!\"\\256\"");
    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_ERROR
    ) << "Error ERROR token";

    EXPECT_EQ(
            lexer->lexer_fsm->lexer_error,
            LEXER_ERROR__STRING_FORMAT
    ) << "Error getting error code";

    provider->setString("!\"\\-1\"");
    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_ERROR
    ) << "Error ERROR token";

    EXPECT_EQ(
            lexer->lexer_fsm->lexer_error,
            LEXER_ERROR__STRING_FORMAT
    ) << "Error getting error code";

    provider->setString("!\\256");
    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_ERROR
    ) << "Error ERROR token";

    EXPECT_EQ(
            lexer->lexer_fsm->lexer_error,
            LEXER_ERROR__STRING_FORMAT
    ) << "Error getting error code";

    provider->setString(R"(!"\512")");
    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_ERROR
    ) << "Error ERROR token";

    EXPECT_EQ(
            lexer->lexer_fsm->lexer_error,
            LEXER_ERROR__STRING_FORMAT
    ) << "Error getting error code";
}

TEST_F(LexerTokenizerTestFixture, ComplexTest) {
    provider->setString("+ <= >= ahoj _8wtf \\ *");

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_ADD
    ) << "Error SMALLER add token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_SMALLER_EQUAL
    ) << "Error SMALLER_EQUAL token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_BIGGER_EQUAL
    ) << "Error BIGGER_EQUAL token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_IDENTIFIER
    ) << "Error IDENTIFIER token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_INTEGER_DIVIDE
    ) << "Error INTEGER_DIVIDE token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_MULTIPLY
    ) << "Error MULTIPLY token";

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_EOF
    ) << "Error EOF token";
}

TEST_F(LexerTokenizerTestFixture, OneLineComment) {
    provider->setString(R"RAW(PrinT 42; ' !"Zadejte cislo pro vypocet faktorialu";
InpuT a
)RAW");

    const std::vector<TokenType> expectedTokens = {
            TOKEN_PRINT, TOKEN_INTEGER_LITERAL, TOKEN_SEMICOLON,
            TOKEN_EOL, TOKEN_INPUT, TOKEN_IDENTIFIER
    };

    for(const TokenType expectedToken: expectedTokens) {
        EXPECT_EQ(
                this->getNextTokenType(),
                expectedToken
        ) << "Error token in complex test";
    }
}

TEST_F(LexerTokenizerTestFixture, StringWithEscapeSequences) {
    provider->setString(R"(!"foob\238armanr545")");

    Token token = lexer_next_token(lexer);
    EXPECT_EQ(
            token.type,
            TOKEN_STRING_VALUE
    );
    EXPECT_STREQ(
            token.data,
            "foob\356armanr545"
    );

    token_free(&token);
}

TEST_F(LexerTokenizerTestFixture, EmptyString) {
    provider->setString(R"(!"")");

    Token token = lexer_next_token(lexer);
    EXPECT_EQ(
            token.type,
            TOKEN_STRING_VALUE
    );
    EXPECT_STREQ(
            token.data,
            ""
    );

    token_free(&token);
}


TEST_F(LexerTokenizerTestFixture, LongString) {
    provider->setString(
            R"(!"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.")");

    EXPECT_EQ(
            this->getNextTokenType(),
            TOKEN_STRING_VALUE
    );
}

TEST_F(LexerTokenizerTestFixture, SecondComplexTest) {
    provider->setString(R"RAW(
/'Program 2: Vypocet faktorialu (rekurzivne)'/
Declare Function factorial (n As Integer) As Integer
Function factorial (n As Integer) As Integer
Dim temp_result As Integer
Dim decremented_n As Integer
Dim result As Integer
If n < 2 Then
result = 1
Else
decremented_n = n - 1
temp_result = factorial(decremented_n)
result = n * temp_result
End If
Return result
End Function
)RAW");

    const std::vector<TokenType> expectedTokens = {
            TOKEN_EOL, TOKEN_EOL, TOKEN_DECLARE, TOKEN_FUNCTION, TOKEN_IDENTIFIER,
            TOKEN_LEFT_BRACKET, TOKEN_IDENTIFIER, TOKEN_AS,
            TOKEN_INTEGER, TOKEN_RIGHT_BRACKET, TOKEN_AS,
            TOKEN_INTEGER, TOKEN_EOL, TOKEN_FUNCTION, TOKEN_IDENTIFIER,
            TOKEN_LEFT_BRACKET, TOKEN_IDENTIFIER, TOKEN_AS,
            TOKEN_INTEGER, TOKEN_RIGHT_BRACKET, TOKEN_AS,
            TOKEN_INTEGER, TOKEN_EOL, TOKEN_DIM, TOKEN_IDENTIFIER,
            TOKEN_AS, TOKEN_INTEGER, TOKEN_EOL, TOKEN_DIM,
            TOKEN_IDENTIFIER, TOKEN_AS, TOKEN_INTEGER, TOKEN_EOL,
            TOKEN_DIM, TOKEN_IDENTIFIER, TOKEN_AS,
            TOKEN_INTEGER, TOKEN_EOL, TOKEN_IF, TOKEN_IDENTIFIER,
            TOKEN_SMALLER, TOKEN_INTEGER_LITERAL, TOKEN_THEN, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_INTEGER_LITERAL, TOKEN_EOL,
            TOKEN_ELSE, TOKEN_EOL, TOKEN_IDENTIFIER, TOKEN_EQUAL,
            TOKEN_IDENTIFIER, TOKEN_SUBTRACT, TOKEN_INTEGER_LITERAL, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER,
            TOKEN_LEFT_BRACKET, TOKEN_IDENTIFIER, TOKEN_RIGHT_BRACKET, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER,
            TOKEN_MULTIPLY, TOKEN_IDENTIFIER, TOKEN_EOL, TOKEN_END,
            TOKEN_IF, TOKEN_EOL, TOKEN_RETURN, TOKEN_IDENTIFIER, TOKEN_EOL,
            TOKEN_END, TOKEN_FUNCTION, TOKEN_EOL, TOKEN_EOF
    };

    for(const TokenType expectedToken: expectedTokens) {
        EXPECT_EQ(
                this->getNextTokenType(),
                expectedToken
        ) << "Error token in complex test";
    }
}

TEST_F(LexerTokenizerTestFixture, ThirdComplexTest) {
    provider->setString(R"RAW(
/' Program 3: Prace s retezci a vestavenymi funkcemi '/
Scope
Dim s1 aS String
Dim s2 As String
Dim s1len As Integer
'Hlavni telo programu
s1 As String
s2 As String
s1len As Integer
s1 = !"Toto je nejaky text"
s2 = s1 + !", ktery jeste trochu obohatime"
Print s1; !"\\n"; s2; !"\\n";
s1len = Length(s1)
s1len = s1len - 4 + 1
s1 = SubStr(s2, s1len, 4)
Print !"4 znaky od "; s1len; !". znaku v \\\""; s2; !"\\\":"; s1; !"\\n";
Print !"Zadejte serazenou posloupnost vsech malych pismen a-h, ";
Print !"pricemz se pismena nesmeji v posloupnosti opakovat";
Input s1
Do While (s1 <> !"abcdefgh")
Print !"\\nSpatne zadana posloupnost, zkuste znovu";
Input s1
Loop
End Scope
)RAW");

    const std::vector<TokenType> expectedTokens = {
            TOKEN_EOL, TOKEN_EOL, TOKEN_SCOPE,
            TOKEN_EOL, TOKEN_DIM, TOKEN_IDENTIFIER,
            TOKEN_AS, TOKEN_STRING, TOKEN_EOL, TOKEN_DIM,
            TOKEN_IDENTIFIER, TOKEN_AS, TOKEN_STRING, TOKEN_EOL,
            TOKEN_DIM, TOKEN_IDENTIFIER, TOKEN_AS,
            TOKEN_INTEGER, TOKEN_EOL, TOKEN_EOL, TOKEN_IDENTIFIER, TOKEN_AS,
            TOKEN_STRING, TOKEN_EOL, TOKEN_IDENTIFIER, TOKEN_AS,
            TOKEN_STRING, TOKEN_EOL, TOKEN_IDENTIFIER, TOKEN_AS,
            TOKEN_INTEGER, TOKEN_EOL, TOKEN_IDENTIFIER, TOKEN_EQUAL,
            TOKEN_STRING_VALUE, TOKEN_EOL, TOKEN_IDENTIFIER, TOKEN_EQUAL,
            TOKEN_IDENTIFIER, TOKEN_ADD, TOKEN_STRING_VALUE, TOKEN_EOL,
            TOKEN_PRINT, TOKEN_IDENTIFIER, TOKEN_SEMICOLON,
            TOKEN_STRING_VALUE, TOKEN_SEMICOLON, TOKEN_IDENTIFIER,
            TOKEN_SEMICOLON, TOKEN_STRING_VALUE, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_LENGTH,
            TOKEN_LEFT_BRACKET, TOKEN_IDENTIFIER, TOKEN_RIGHT_BRACKET, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER,
            TOKEN_SUBTRACT, TOKEN_INTEGER_LITERAL, TOKEN_ADD,
            TOKEN_INTEGER_LITERAL, TOKEN_EOL, TOKEN_IDENTIFIER, TOKEN_EQUAL,
            TOKEN_SUBSTR, TOKEN_LEFT_BRACKET, TOKEN_IDENTIFIER,
            TOKEN_COMMA, TOKEN_IDENTIFIER, TOKEN_COMMA,
            TOKEN_INTEGER_LITERAL, TOKEN_RIGHT_BRACKET, TOKEN_EOL,
            TOKEN_PRINT, TOKEN_STRING_VALUE, TOKEN_SEMICOLON,
            TOKEN_IDENTIFIER, TOKEN_SEMICOLON, TOKEN_STRING_VALUE,
            TOKEN_SEMICOLON, TOKEN_IDENTIFIER, TOKEN_SEMICOLON,
            TOKEN_STRING_VALUE, TOKEN_SEMICOLON, TOKEN_IDENTIFIER,
            TOKEN_SEMICOLON, TOKEN_STRING_VALUE, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_PRINT, TOKEN_STRING_VALUE, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_PRINT, TOKEN_STRING_VALUE, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_INPUT, TOKEN_IDENTIFIER, TOKEN_EOL, TOKEN_DO,
            TOKEN_WHILE, TOKEN_LEFT_BRACKET, TOKEN_IDENTIFIER,
            TOKEN_SMALLER_BIGGER, TOKEN_STRING_VALUE, TOKEN_RIGHT_BRACKET, TOKEN_EOL,
            TOKEN_PRINT, TOKEN_STRING_VALUE, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_INPUT, TOKEN_IDENTIFIER, TOKEN_EOL, TOKEN_LOOP, TOKEN_EOL,
            TOKEN_END, TOKEN_SCOPE, TOKEN_EOL, TOKEN_EOF
    };


    for(const TokenType expectedToken: expectedTokens) {
        EXPECT_EQ(
                this->getNextTokenType(),
                expectedToken
        ) << "Error token in complex test";
    }
}
