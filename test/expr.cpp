#include <utility>

#include "gtest/gtest.h"
#include "utils/stringbycharprovider.h"


extern "C" {
#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/memory.h"
#include "../src/parser_expr.h"
}

class ParserExpressionTestFixture : public ::testing::Test {
    protected:
        Parser* parser;
        StringByCharProvider* provider;

        void SetUp() override {
            parser = parser_init(token_stream);
            parser->run_type = PARSER_RUN_TYPE_NOTHING;
            provider = StringByCharProvider::instance();
        }

        void TearDown() override {
            parser_free(&parser);
        }
};

TEST_F(ParserExpressionTestFixture, Constant) {
    // Rule is Epsilon, it is just demonstration, it will be implemented in future
    provider->setString("31 begin");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            lexer_next_token(parser->lexer).type,
            TOKEN_SCOPE
    ) << "Error get token after <expression> rule";
}


TEST_F(ParserExpressionTestFixture, Variable) {
    // Rule is Epsilon, it is just demonstration, it will be implemented in future
    provider->setString("variable )");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            lexer_next_token(parser->lexer).type,
            TOKEN_RIGHT_BRACKET
    ) << "Error get token after <expression> rule";


}

TEST_F(ParserExpressionTestFixture, AddConstatnts) {
    // Rule is Epsilon, it is just demonstration, it will be implemented in future
    provider->setString("30 + 30 scope" );

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
        lexer_next_token(parser->lexer).type,
        TOKEN_SCOPE
    ) << "Error get token after <expression> rule";
}

TEST_F(ParserExpressionTestFixture, AddConstatntsFalse) {
    // Rule is Epsilon, it is just demonstration, it will be implemented in future
    provider->setString("(30+30");

    EXPECT_FALSE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";
}

TEST_F(ParserExpressionTestFixture, AddVariablesFalse) {
    // Rule is Epsilon, it is just demonstration, it will be implemented in future
    provider->setString("(djkg+jlkdh7djkljhl");

    EXPECT_FALSE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";
}






