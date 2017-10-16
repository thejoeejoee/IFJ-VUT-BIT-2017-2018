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
        Token tmp;

        void SetUp() override {
            parser = parser_init(token_stream);
            parser->run_type = PARSER_RUN_TYPE_NOTHING;
            provider = StringByCharProvider::instance();
        }

        void TearDown() override {
            token_free(&tmp);
            parser_free(&parser);
        }
};

TEST_F(ParserExpressionTestFixture, Constants) {

    provider->setString("33 begin");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_IDENTIFIER
    ) << "Error get token after <expression> rule";
}

TEST_F(ParserExpressionTestFixture, Constants2) {

    provider->setString("31 + 658 + 67896 + 67876897 \n");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";
}


TEST_F(ParserExpressionTestFixture, Variable) {

    provider->setString("variable \n");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";


}

TEST_F(ParserExpressionTestFixture, SimpleRelation1) {

    provider->setString("a >= b \n");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";


}

TEST_F(ParserExpressionTestFixture, SimpleRelation2) {

    provider->setString("a <= b \n");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";


}

TEST_F(ParserExpressionTestFixture, SimpleRelation3) {

    provider->setString("a = b \n");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";


}

TEST_F(ParserExpressionTestFixture, SimpleRelation4) {

    provider->setString("a >= b \n");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";


}

TEST_F(ParserExpressionTestFixture, SimpleRelation5) {

    provider->setString("a <> b \n");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";


}


TEST_F(ParserExpressionTestFixture, AddConstatnts) {

    provider->setString("30 + 30 scope" );

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
        (tmp = lexer_next_token(parser->lexer)).type,
        TOKEN_SCOPE
    ) << "Error get token after <expression> rule";
}

TEST_F(ParserExpressionTestFixture, ConstantsAndBrackets) {

    provider->setString("((30 + 10) * 10) / 32 \n" );

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";
}

TEST_F(ParserExpressionTestFixture, AddConstatntsFalse) {

    provider->setString("(30+30");

    EXPECT_FALSE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

}

TEST_F(ParserExpressionTestFixture, SimpleOperations) {

    provider->setString("12-10+213*432/3432 \n");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";


}

TEST_F(ParserExpressionTestFixture, AddVariablesFalse) {

    provider->setString("(sdgjhads+sdjkgsaljkdk");

    EXPECT_FALSE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

}

TEST_F(ParserExpressionTestFixture, AddVariablesFalse2) {

    provider->setString("(djkg+jlkdh7djkljhl");

    EXPECT_FALSE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";
}