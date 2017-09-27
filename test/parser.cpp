#include <utility>

#include "gtest/gtest.h"
#include "utils/stringbycharprovider.h"


extern "C" {
#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/memory.h"
}

class ParserTestFixture : public ::testing::Test {
    protected:
        Parser *parser;
        StringByCharProvider* provider;

        void SetUp() override {
            parser = parser_init(token_stream);
            provider = StringByCharProvider::instance();
        }

        void TearDown() override {
            parser_free(&parser);
        }
};


TEST_F(ParserTestFixture, BodyRule) {
    provider->setString("SCOPE END SCOPE");
    EXPECT_TRUE(
            parser_parse_body(parser)
    ) << "Body parse";

    provider->setString("SCOPE ENDD SCOPE");
    EXPECT_FALSE(
            parser_parse_body(parser)
    ) << "Error body parse";

    provider->setString("END SCOPE");
    EXPECT_FALSE(
            parser_parse_body(parser)
    ) << "Error body parse";

    provider->setString("SCOPE END");
    EXPECT_FALSE(
            parser_parse_body(parser)
    ) << "Error body parse";

    provider->setString("SCOPE SCOPE END SCOPE");
    EXPECT_FALSE(
            parser_parse_body(parser)
    ) << "Error body parse";
}

TEST_F(ParserTestFixture, FuntionParama) {

    // Rule is Epsilon, it is just demonstration, it will be implemented in future
    provider->setString("ahoj as string");

    EXPECT_TRUE(
            parser_function_param(parser)
    ) << "Error parsing <funtion_param> rule";
}

TEST_F(ParserTestFixture, FunctionHeader) {

    /**
     * RULE
     * FUNCTION IDENTIFIER (<function_params>) AS <type>
     */

    provider->setString("FUNCTION hello () AS string");

    EXPECT_TRUE(
            parser_parse_function_header(parser)
    ) << "Error parsing <funtion_header> rule";

    provider->setString("FUNCTION WELLCOME () ASC string");

    EXPECT_FALSE(
            parser_parse_function_header(parser)
    ) << "Error parsing <funtion_header> rule";
}

TEST_F(ParserTestFixture, FunctionDeclaration) {

    /**
     * RULE
     * <function_declaration> -> DECLARE <function_header> EOL <eols>
     */

    provider->setString("DECLARE FUNCTION hello () AS string \n");

    EXPECT_TRUE(
            parser_parse_function_declaration(parser)
    ) << "Error parsing <funtion_header> rule";

    char_stack_empty(parser->lexer->lexer_fsm->stack);

    parser->lexer->stack_token = NULL;

    provider->setString("DECLARE FUNCTION hello () AS string \n \n \n");

    EXPECT_TRUE(
            parser_parse_function_declaration(parser)
    ) << "Error parsing <funtion_header> rule";
}

TEST_F(ParserTestFixture, Definitions) {

    // Rule is Epsilon, it is just demonstration, it will be implemented in future
    provider->setString("");

    EXPECT_TRUE(
            parser_parse_definitions(parser)
    ) << "Error parsing <definitions> rule";
}

TEST_F(ParserTestFixture, Statements) {

    // Rule is Epsilon, it is just demonstration, it will be implemented in future
    provider->setString("");

    EXPECT_TRUE(
            parser_parse_statements(parser)
    ) << "Error parsing <statements> rule";
}