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
            parser->parser_semantic->expression_result->data_type,
            DATA_TYPE_INTEGER
    );

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_IDENTIFIER
    ) << "Error get token after <expression> rule";
}

TEST_F(ParserExpressionTestFixture, Constants2) {

    provider->setString("31 + 658 + 67896 + 67876897 )");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_RIGHT_BRACKET
    ) << "Error get token after <expression> rule";
}


TEST_F(ParserExpressionTestFixture, Variable) {

    provider->setString("variable )");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_RIGHT_BRACKET
    ) << "Error get token after <expression> rule";


}

TEST_F(ParserExpressionTestFixture, SimpleRelation1) {

    provider->setString("4 >= 4.4 \n");

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
    provider->setString("3 = 3 \n");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";
}

TEST_F(ParserExpressionTestFixture, SimpleRelation4) {
    provider->setString("4 >= 3 \n");
    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";
    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";
}

TEST_F(ParserExpressionTestFixture, SimpleRelation5) {
    provider->setString("1 < 2 then");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_THEN
    ) << "Error get token after <expression> rule";
}


TEST_F(ParserExpressionTestFixture, SimpleRelation6) {
    provider->setString("3 <> 4 \n");
    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";
    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";
}


TEST_F(ParserExpressionTestFixture, AddConstants) {
    provider->setString("30 + 30 scope");
    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";
    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_SCOPE
    ) << "Error get token after <expression> rule";
}

TEST_F(ParserExpressionTestFixture, ConstantsAndBrackets) {
    provider->setString("((30 + 10) * 10) / 32 \n");
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

TEST_F(ParserExpressionTestFixture, CallFunction) {

    provider->setString("GGHKGHHG_56566(42) )");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_RIGHT_BRACKET
    ) << "Error get token after <expression> rule";

}

TEST_F(ParserExpressionTestFixture, CallFunctionVariableArgument) {

    provider->setString("factorial(decremented_n) \n");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";

}


TEST_F(ParserExpressionTestFixture, CallFunctionWithMoreAarguments) {

    provider->setString("GFGFGHGHHK5686867(567, 5677) )");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_RIGHT_BRACKET
    ) << "Error get token after <expression> rule";

}

TEST_F(ParserExpressionTestFixture, AddFunctions) {

    provider->setString("guzghh(43) + hdsghkgh(6767) )");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_RIGHT_BRACKET
    ) << "Error get token after <expression> rule";

}

TEST_F(ParserExpressionTestFixture, MultiplyFunctions) {

    provider->setString("ASDFGHZTGRFTH(43) * jfkdmvnfrjdshkkkjiuudtz(6767) )");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_RIGHT_BRACKET
    ) << "Error get token after <expression> rule";

}

TEST_F(ParserExpressionTestFixture, DivideFunctions) {

    provider->setString("GHKHJ7898L(43) * EWRWT78990kjk(6767) )");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_RIGHT_BRACKET
    ) << "Error get token after <expression> rule";
}

TEST_F(ParserExpressionTestFixture, StringSimple) {

    provider->setString(" !\" Vysledek je \" )");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            parser->parser_semantic->expression_result->data_type,
            DATA_TYPE_STRING
    );

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_RIGHT_BRACKET
    ) << "Error get token after <expression> rule";
}


TEST_F(ParserExpressionTestFixture, UnaryMinus0) {

    provider->setString(" -48 \n");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            parser->parser_semantic->expression_result->data_type,
            DATA_TYPE_INTEGER
    );

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";
}


TEST_F(ParserExpressionTestFixture, SooooooooHardcooooooreExpressionI) {

    provider->setString("funkce(hgdk_568(j_(42) * 42) / 42 ) * 10 / GHKGJ67867_568) )");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_RIGHT_BRACKET
    ) << "Error get token after <expression> rule";
}


TEST_F(ParserExpressionTestFixture, SooooooooHardcooooooreExpressionII) {
    provider->setString(
            "funkce(hgdk_568(j_(42) * hgdk_568(4, 0 + foobar865789) + 2, hgdk_5687745(785)) / hgdk_5687474(hgdk_568(42))) * 10 / GHKGJ67867_568 + 78 \n");

    EXPECT_TRUE(
            parser_parse_expression(parser)
    ) << "Error parsing <expression> rule";

    EXPECT_EQ(
            (tmp = lexer_next_token(parser->lexer)).type,
            TOKEN_EOL
    ) << "Error get token after <expression> rule";
}







