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
            parser_parse_statement_single(parser)
    ) << "Error parsing <definitions> rule";

}

TEST_F(ParserSemanticTestFixture, FunctionDeclaration) {
    provider->setString("dim promena785 as integer");

    EXPECT_TRUE(
            parser_parse_variable_declaration(parser)
    ) << "Error parsing program";

}

TEST_F(ParserSemanticTestFixture, FunctionDefinition) {
    provider->setString(R"(
declare function foo735() as integer
function foo735() as integer
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
function fooqwe() as integer
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
function foowebtg() as integer
end function
function foowebtg() as string
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
declare function fooyxcvyxvc() as integer
function fooyxcvyxvc() as integer
end function
function fooyxcvyxvc() as integer
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
declare function fooqwead() as integer
function fooqwead() as string
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
function erzert(aqrqwer as integer, bqewqxc as integer) as integer
end function
function erzert(aqrqwer as integer) as integer
end function
scope
end scope
    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";

}


TEST_F(ParserSemanticTestFixture, FunctionDefinitionWithoutDeclaration) {
    provider->setString(R"(
function erzert(aqrqwer as integer, bqewqxc as integer) as integer
end function

scope
end scope
    )");

    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Error parsing program";

}

TEST_F(ParserSemanticTestFixture, FunctionInvalidCount) {
    provider->setString(R"(
' function params with declaration but invalid count
declare function bar(h as string, t as integer, x as string) as string
function bar(b as string, c as integer) as string
end function
scope
end scope

    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";

}


TEST_F(ParserSemanticTestFixture, FunctionDuplicityArgumentName) {
    provider->setString(R"(

function bar(h as string, h as integer, x as string) as string
end function

scope
end scope

    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";

}

TEST_F(ParserSemanticTestFixture, FunctionDuplicityArgumentName2) {
    provider->setString(R"(

declare function bar(h as string, a as integer, h as string) as string

function bar(h as string, a as integer, x as string) as string
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
declare function qweadscyx(iopfg as string) as integer
function qweadscyx(iopfg as string) as integer
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
declare function mikmikmik(qweqweqwe as string, rfvrfvrfv as integer) as integer
function mikmikmik(qweqweqwe as string, rfvrfvrfv as integer) as integer
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
declare function thht(yeyeyesyse as string) as integer
function thht(yeyeyesyse as integer) as integer
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
declare function qaywsxedc(njznz as string, yxcyxcyxc as integer) as integer
function qaywsxedc(njznz as string, yxcyxcyxc as string) as integer
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
declare function yrse(tztzujzuj as string, qwexcxcyb as integer) as integer
function yrse(tztzujzuj as string, qwexcxcyb as integer, qweasdcyxv as integer) as integer
end function
scope
end scope
    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";
}

TEST_F(ParserSemanticTestFixture, DeclareFunctionWithoutDefitiniton) {
    provider->setString(R"(
declare function foo_complex3qwe(aqewqwe as integer) as integer

SCOPE
END SCOPE
    )");
    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Body parse";
}

TEST_F(ParserSemanticTestFixture, ComplexTest1) {
    provider->setString(R"(scope
input qweeqwrsdf
dim qweeqwrsdf as integer
end scope
    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";

}

TEST_F(ParserSemanticTestFixture, ComplexTest2) {
    provider->setString(R"(scope
dim cbnfjtzu as integer
input cbnfjtzu
end scope
    )");

    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Error parsing program";
}

TEST_F(ParserSemanticTestFixture, ComplexTest3) {
    provider->setString(R"(scope
dim ywergdfgert as integer
input ywergdfgert
dim ywergdfgert as string
end scope
    )");

    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Error parsing program";

}

TEST_F(ParserSemanticTestFixture, ComplexTest4) {
    provider->setString(R"(
declare function foo_function_complexqwead(aqwead as integer) as integer
declare function foo_function_complexlpp(aqwead as string) as string
SCOPE
END SCOPE
    )");
    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Body parse";

}

TEST_F(ParserSemanticTestFixture, ComplexTest4WithDefinitions) {
    provider->setString(R"(
declare function foo_function_complexqwead(aqwead as integer) as integer
declare function foo_function_complexlpp(aqwead as string) as string

function foo_function_complexqwead(aqwead as integer) as integer
print 42;
end function

function foo_function_complexlpp(aqwead as string) as string
dim a as integer
input a
end function

SCOPE
END SCOPE
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

}

TEST_F(ParserSemanticTestFixture, ComplexTest5) {
    provider->setString(R"(
declare function foo_complex3qwe(aqewqwe as integer) as integer
declare function foo_complex3qwe(aqewqwe as string) as string
SCOPE
END SCOPE
    )");
    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Body parse";
}

