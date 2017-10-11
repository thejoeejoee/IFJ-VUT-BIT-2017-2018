#include <utility>

#include "gtest/gtest.h"
#include "utils/stringbycharprovider.h"


extern "C" {
#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/memory.h"
#include "../src/parser_expr.h"
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


    provider->setString(
            R"(
                SCOPE





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

    provider->setString("FUNCTION hello (param1 as string) AS integer");
    EXPECT_TRUE(
            parser_parse_function_header(parser)
    ) << "Error parsing <funtion_header> rule";

    provider->setString("FUNCTION hello (param2 as string, param6 as integer) AS integer");
    EXPECT_TRUE(
            parser_parse_function_header(parser)
    ) << "Error parsing <funtion_header> rule";

    provider->setString("FUNCTION hello (param3 as string, param4 as integer, param5 as double) AS integer");
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

TEST_F(ParserTestFixture, PrintStatement) {
    // Rule is Epsilon, it is just demonstration, it will be implemented in future
    provider->setString("print 42;\n");

    EXPECT_TRUE(
            parser_parse_print(parser)
    ) << "Error parsing <print> rule";

    parser->lexer->is_token_rewind = false;

    provider->setString("print 42; 24; 42; \n");

    EXPECT_TRUE(
            parser_parse_print(parser)
    ) << "Error parsing <print> rule with concatenation";

    parser->lexer->is_token_rewind = false;

    provider->setString("print 42;42;42;42;42;42; \n");

    EXPECT_TRUE(
            parser_parse_print(parser)
    ) << "Error parsing <print> rule with concatenation";
}

TEST_F(ParserTestFixture, BodyStatementSingle) {
    provider->setString("input foo");

    EXPECT_TRUE(
            parser_parse_body_statement_single(parser)
    ) << "Error parsing <definitions> rule";

}

TEST_F(ParserTestFixture, IfRule) {
    provider->setString(R"(if 42 then
input id
elseif 43
input id
else
input id
input id
end if
    )");

    EXPECT_TRUE(
            parser_parse_condition(parser)
    ) << "Error parsing <condition> rule";

}

TEST_F(ParserTestFixture, DimRule) {

    provider->setString("dim param475624 as string");

    EXPECT_TRUE(
            parser_parse_variable_declaration(parser)
    ) << "Error parsing <variable_declaration> rule";

    provider->setString("dim param48745 as integer");

    parser_semantic_free(&(parser->parser_semantic));

    parser->parser_semantic = parser_semantic_init();

    EXPECT_TRUE(
            parser_parse_variable_declaration(parser)
    ) << "Error parsing <variable_declaration> rule";

    provider->setString("dim param79541 asc string");

    EXPECT_FALSE(
            parser_parse_variable_declaration(parser)
    ) << "Error parsing <variable_declaration> rule";


}

TEST_F(ParserTestFixture, DoWhile) {

    provider->setString(R"(DO WHILE 42
input id
DO WHILE 42
input id
input id
loop
input id
loop
    )");

    parser->body_statement = true;

    EXPECT_TRUE(
            parser_parse_while_(parser)
    ) << "Error parsing <do_while> rule";

    parser->body_statement = false;


}

TEST_F(ParserTestFixture, ReturnRule) {


    provider->setString("return 34");

    EXPECT_TRUE(
            parser_parse_return_(parser)
    ) << "Error parsing <expression> rule";

}

TEST_F(ParserTestFixture, ComplexTest) {
    provider->setString(R"(
SCOPE

input id
print 42;

END SCOPE
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

    provider->setString(R"(
SCOPE
DO WHILE 42
input id
input id
loop
END SCOPE
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";


}

TEST_F(ParserTestFixture, ComplexTest2) {
    provider->setString(R"(
declare function foo_function(a as integer, b as string) as string
function bar(b as string, a as integer) as integer
dim a as integer
input a
print 42;42;332;
end function

SCOPE

SCOPE
if 32 then
input id
dim a as string
end if
END SCOPE

END SCOPE
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

}

TEST_F(ParserTestFixture, ComplexTestWhileInFuncion) {
    provider->setString(R"(
FUNCTION FOO() AS INTEGER
DO WHILE 42
input id
input id
loop
END  FUNCTION
SCOPE
DO WHILE 42
input id
input id
loop
END SCOPE
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

}

TEST_F(ParserTestFixture, ComplexTestConditionInFuncion) {
    provider->setString(R"(
FUNCTION FOO() AS INTEGER
DO WHILE 42
if 42 then
input id
elseif 43
input id
else
input id
input id
end if
loop
END  FUNCTION
SCOPE
DO WHILE 42
input id
input id
loop
END SCOPE
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

    provider->setString(R"(
FUNCTION FOO() AS INTEGER
DO WHILE 42
if 42 then
input id
elseif 43
input id
else
input id
input id
end if
loop
END  FUNCTION
SCOPE
DO WHILE 42
input id
input id
loop
RETURN 43
END SCOPE
    )");
    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Body parse";



}

TEST_F(ParserTestFixture, ComplexTestWithCondition) {
    provider->setString(R"(
SCOPE
if 42 then
input id
elseif 43
input id
else
input id
input id
end if
END SCOPE
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

    provider->setString(R"(
SCOPE
return 32
if 42 then
input id
elseif 43
input id
else
input id
input id
end if
END SCOPE
    )");
    EXPECT_FALSE(
            parser_parse_program(parser)
    ) << "Body parse";

}

TEST_F(ParserTestFixture, ComplexTestWithScopeInsideScope) {
    provider->setString(R"(
SCOPE
input id

SCOPE
input id
END SCOPE

END SCOPE
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

}





