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


TEST_F(ParserTestFixture, BodyRule) {
    provider->setString("SCOPE \n END SCOPE");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

    provider->setString("DECLARE FUNCTION hello () AS string \n SCOPE \n END SCOPE");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

    provider->setString("DECLARE FUNCTION hello () AS string \n \n \n SCOPE \n END SCOPE");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

    provider->setString("DECLARE FUNCTION hello () AS string \n "
                                "DECLARE FUNCTION foo () AS integer\n \n SCOPE \n END SCOPE");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

    provider->setString("DECLARE FUNCTION hello () AS string \n "
                                "\n FUNCTION hello () AS string \n"
                                " input id \n"
                                " input id \n"
                                " END FUNCTION \n"
                                "\n SCOPE \n END SCOPE");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

    provider->setString("Scope \n "
                                "input id \n"
                                "END SCOPE \n");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

    provider->setString("Scope \n "
                                "DIM PROMENA AS INTEGER \n"
                                "END SCOPE \n");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

    provider->setString(
            R"(DECLARE FUNCTION hello () AS string
                FUNCTION hello () AS string
                    input id
                    input id
                END FUNCTION
                SCOPE
                    input foo
                    INput bar
                    input test
                END SCOPE
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";


    provider->setString("SCOPE \n ENDD SCOPE");
    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error body parse";

    provider->setString("END SCOPE");
    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error body parse";

    provider->setString("SCOPE END");
    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error body parse";

    provider->setString("SCOPE SCOPE END SCOPE");
    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error body parse";
}

TEST_F(ParserTestFixture, FuntionParama) {
    // Rule is Epsilon, it is just demonstration, it will be implemented in future
    provider->setString("foo as string");
    EXPECT_TRUE(
            parser_parse_function_param(parser)
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

    provider->setString("FUNCTION hello () AS integer");
    EXPECT_TRUE(
            parser_parse_function_header(parser)
    ) << "Error parsing <funtion_header> rule";

    provider->setString("FUNCTION hello (foo as string) AS integer");
    EXPECT_TRUE(
            parser_parse_function_header(parser)
    ) << "Error parsing <funtion_header> rule";

    provider->setString("FUNCTION hello (foo as string, bar as integer) AS integer");
    EXPECT_TRUE(
            parser_parse_function_header(parser)
    ) << "Error parsing <funtion_header> rule";

    provider->setString("FUNCTION hello (foo as string, bar as integer, foobar as double) AS integer");
    EXPECT_TRUE(
            parser_parse_function_header(parser)
    ) << "Error parsing <funtion_header> rule";

    provider->setString("FUNCTION WELCOME () ASC integer");
    EXPECT_FALSE(
            parser_parse_function_header(parser)
    ) << "Error parsing <funtion_header> rule";

    provider->setString("FUNCTION WELCOME (foo as string) ASC string");
    EXPECT_FALSE(
            parser_parse_function_header(parser)
    ) << "Error parsing <funtion_header> rule";
}


TEST_F(ParserTestFixture, FunctionDeclaration) {
    /**
     * RULE
     * <function_declaration> -> DECLARE <function_header> EOL <eols>
     */
    provider->setString("DECLARE FUNCTION hello () AS string \n \t \n ");
    EXPECT_TRUE(
            parser_parse_function_declaration(parser)
    ) << "Error parsing <funtion_header> rule";
}

TEST_F(ParserTestFixture, FunctionMultipleDeclaration) {
    provider->setString(R"(DECLARE FUNCTION hello () AS string

DECLARE FUNCTION hello () AS string

)");
    EXPECT_TRUE(
            parser_parse_function_declaration(parser)
    ) << "Error parsing <funtion_header> rule";

}

TEST_F(ParserTestFixture, Definitions) {
    provider->setString("");
    EXPECT_TRUE(
            parser_parse_definitions(parser)
    ) << "Error parsing <definitions> rule";

    provider->setString("");
    EXPECT_TRUE(
            parser_parse_definitions(parser)
    ) << "Error parsing <definitions> rule";

    provider->setString("DECLARE FUNCTION hello () AS string");
    EXPECT_TRUE(
            parser_parse_definitions(parser)
    ) << "Error parsing <definitions> rule";

}

TEST_F(ParserTestFixture, FunctionDefinition) {
    provider->setString("FUNCTION hello () AS string \n END FUNCTION");
    EXPECT_TRUE(
            parser_parse_definitions(parser)
    ) << "Error parsing <definitions> rule";

    provider->setString(R"(FUNCTION hello () AS string
input id
input id
END FUNCTION
    )");
    EXPECT_TRUE(
            parser_parse_definitions(parser)
    ) << "Error parsing <definitions> rule";

}

TEST_F(ParserTestFixture, FunctionStatements) {
    provider->setString("");
    EXPECT_TRUE(
            parser_parse_function_statements(parser)
    ) << "Error parsing <function_statements> rule";

    provider->setString("input id \n");
    EXPECT_TRUE(
            parser_parse_function_statements(parser)
    ) << "Error parsing <function_statements> rule";

    provider->setString("input id \n input id \n input id \n");
    EXPECT_TRUE(
            parser_parse_function_statements(parser)
    ) << "Error parsing <function_statements> rule";
}

TEST_F(ParserTestFixture, FunctionStatementSingle) {
    // Rule is Epsilon, it is just demonstration, it will be implemented in future
    provider->setString("input foo");

    EXPECT_TRUE(
            parser_parse_function_statement_single(parser)
    ) << "Error parsing <definitions> rule";

}

TEST_F(ParserTestFixture, BodyStatementSingle) {
    // Rule is Epsilon, it is just demonstration, it will be implemented in future
    provider->setString("input foo");

    EXPECT_TRUE(
            parser_parse_body_statement_single(parser)
    ) << "Error parsing <definitions> rule";

}

TEST_F(ParserTestFixture, DimRule) {

    provider->setString("dim promena as string");

    EXPECT_TRUE(
            parser_parse_variable_declaration(parser)
    ) << "Error parsing <variable_declaration> rule";

    provider->setString("dim id as integer");

    parser_semantic_free(&(parser->parser_semantic));

    parser->parser_semantic = parser_semantic_init();

    EXPECT_TRUE(
            parser_parse_variable_declaration(parser)
    ) << "Error parsing <variable_declaration> rule";

    provider->setString("dim id asc string");

    EXPECT_FALSE(
            parser_parse_variable_declaration(parser)
    ) << "Error parsing <variable_declaration> rule";


}