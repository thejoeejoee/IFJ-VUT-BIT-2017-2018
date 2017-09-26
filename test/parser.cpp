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



TEST_F(ParserTestFixture, FuntionParama) {

    /**
     * At first, we have to test rules that are the lowest in the tree
     */

    // Rule is Epsilon, it is just demonstration, it will be implemented in future
    provider->setString("ahoj as string");

    EXPECT_TRUE(
            parser_function_param(parser)
    ) << "Error parsing <funtion_param> rule";
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