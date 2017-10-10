#include <utility>

#include "gtest/gtest.h"
#include "utils/stringbycharprovider.h"


extern "C" {
#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/memory.h"
}

class ParserSemanticTestFixture : public ::testing::Test {
    protected:
        Parser* parser;
        StringByCharProvider* provider;

        void SetUp() override {
            parser = parser_init(token_stream);
            parser->run_type = PARSER_RUN_TYPE_SEMANTIC_ANALYSIS;
            provider = StringByCharProvider::instance();
        }

        void TearDown() override {
            parser_free(&parser);
        }
};

TEST_F(ParserSemanticTestFixture, FunctionStatementSingle) {
    symbol_register_push_variables_table(parser->parser_semantic->register_);
    symbol_table_get_or_create(parser->parser_semantic->register_->variables->symbol_table, "hello_input42");

    provider->setString("input hello_input42");

    EXPECT_TRUE(
            parser_parse_function_statement_single(parser)
    ) << "Error parsing <definitions> rule";

}

TEST_F(ParserSemanticTestFixture, FunctionDeclaration) {
    provider->setString("dim promena as integer");

    EXPECT_TRUE(
            parser_parse_variable_declaration(parser)
    ) << "Error parsing program";

}

TEST_F(ParserSemanticTestFixture, FunctionDefinition) {
    provider->setString(R"(
declare function foo() as integer
function foo() as integer
end function
scope
end scope
    )");

    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Error parsing program";
}

TEST_F(ParserSemanticTestFixture, FunctionDefinition1) {
    provider->setString(R"(
function foo() as integer
end function
scope
end scope
    )");

    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Error parsing program";

}

TEST_F(ParserSemanticTestFixture, FunctionDefinition2) {
    provider->setString(R"(
function foo() as integer
end function
function foo() as string
end function
scope
end scope
    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";

}

TEST_F(ParserSemanticTestFixture, FunctionDefinition3) {
    provider->setString(R"(
declare function foo() as integer
function foo() as integer
end function
function foo() as integer
end function
scope
end scope
    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";

}

TEST_F(ParserSemanticTestFixture, FunctionDefinition4) {
    provider->setString(R"(
declare function foo() as integer
function foo() as string
end function
scope
end scope
    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";
}

TEST_F(ParserSemanticTestFixture, FunctionDefinition5) {
    provider->setString(R"(
function foo(a as integer, b as integer) as integer
end function
function foo(a as integer) as integer
end function
scope
end scope
    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";

}

TEST_F(ParserSemanticTestFixture, FunctionDecAndDefWithParams) {
    provider->setString(R"(
declare function foo(a as string) as integer
function foo(a as string) as integer
end function
scope
end scope
    )");

    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Error parsing program";
}


TEST_F(ParserSemanticTestFixture, FunctionDecAndDefWithParams2) {
    provider->setString(R"(
declare function foo(a as string, b as integer) as integer
function foo(a as string, b as integer) as integer
end function
scope
end scope
    )");

    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Error parsing program";
}

TEST_F(ParserSemanticTestFixture, FunctionDecAndDefWithParamsBad) {

    provider->setString(R"(
declare function foo(a as string) as integer
function foo(a as integer) as integer
end function
scope
end scope
    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";
}

TEST_F(ParserSemanticTestFixture, FunctionDecAndDefWithParamsBad2) {

    provider->setString(R"(
declare function foo(a as string, b as integer) as integer
function foo(a as string, b as string) as integer
end function
scope
end scope
    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";
}

TEST_F(ParserSemanticTestFixture, FunctionDecAndDefWithParamsBad3) {
    provider->setString(R"(
declare function foo(a as string, b as integer) as integer
function foo(a as string, b as integer, c as integer) as integer
end function
scope
end scope
    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";
}

TEST_F(ParserSemanticTestFixture, ComplexTest1) {
    provider->setString(R"(scope
input promena
dim promena as integer
end scope
    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";

}

TEST_F(ParserSemanticTestFixture, ComplexTest2) {
    provider->setString(R"(scope
dim promena as integer
input promena
end scope
    )");

    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Error parsing program";
}

TEST_F(ParserSemanticTestFixture, ComplexTest3) {
    provider->setString(R"(scope
dim promena as integer
input promena
dim promena as string
end scope
    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";

}

TEST_F(ParserSemanticTestFixture, ComplexTest4) {
    provider->setString(R"(
declare function foo_function_complex(a as integer) as integer
declare function bar_function_complex(a as string) as string
SCOPE
END SCOPE
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

}

TEST_F(ParserSemanticTestFixture, ComplexTest5) {
    provider->setString(R"(
declare function foo_complex3(a as integer) as integer
declare function foo_complex3(a as string) as string
SCOPE
END SCOPE
    )");
    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Body parse";
}

