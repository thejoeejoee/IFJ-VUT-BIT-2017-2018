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

}

TEST_F(ParserTestFixture, PrintStatement2) {
    provider->setString("print 42; 24; 42; \n");

    EXPECT_TRUE(
            parser_parse_print(parser)
    ) << "Error parsing <print> rule with concatenation";

}

TEST_F(ParserTestFixture, PrintStatement3) {
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

TEST_F(ParserTestFixture, IfRule0) {
    provider->setString(R"(if 714156 then
print 4658791;
end if
    )");

    EXPECT_TRUE(
            parser_parse_condition(parser)
    ) << "Error parsing <condition> rule";
}

TEST_F(ParserTestFixture, IfRule1) {
    provider->setString(R"(if 46593 then
print 1254;
else
print 23137486;
print 214687;
end if
    )");

    EXPECT_TRUE(
            parser_parse_condition(parser)
    ) << "Error parsing <condition> rule";
}

TEST_F(ParserTestFixture, IfRule2) {
    provider->setString(R"(if 42 then
print 23137213546486;
print 23137213546486;
elseif 43 then
if 38 then
print 23137879486;
elseif 14 then
print 23137213546486;
end if
else
print 23137213546486;
print 23137213546486;
end if
    )");

    EXPECT_TRUE(
            parser_parse_condition(parser)
    ) << "Error parsing <condition> rule";
}

TEST_F(ParserTestFixture, DimRuleDeclaration) {

    provider->setString("dim param475624 as string");

    EXPECT_TRUE(
            parser_parse_variable_declaration(parser)
    ) << "Error parsing <variable_declaration> rule";


}

TEST_F(ParserTestFixture, DimRuleDeclaration2) {

    provider->setString("dim param48745 as integer");

    EXPECT_TRUE(
            parser_parse_variable_declaration(parser)
    ) << "Error parsing <variable_declaration> rule";


}

TEST_F(ParserTestFixture, DimRuleDeclaration3) {


    provider->setString("dim param79541 asc string");

    EXPECT_FALSE(
            parser_parse_variable_declaration(parser)
    ) << "Error parsing <variable_declaration> rule";


}

TEST_F(ParserTestFixture, DimRuleDeclaration4) {

    provider->setString("dim param475624 as integer = 42");

    EXPECT_TRUE(
            parser_parse_variable_declaration(parser)
    ) << "Error parsing <variable_declaration> rule";


}


TEST_F(ParserTestFixture, DimRuleWithAssignment) {

    provider->setString("dim param475624 as integer = 10");

    EXPECT_TRUE(
            parser_parse_variable_declaration(parser)
    ) << "Error parsing <variable_declaration> rule";


}


TEST_F(ParserTestFixture, AssignmentRule) {

    provider->setString("= 56875687");

    EXPECT_TRUE(
            parser_parse_assignment(parser)
    ) << "Error parsing <assignment rule";


}


TEST_F(ParserTestFixture, DeclarationAssigment) {

    provider->setString(" = 567868");

    EXPECT_TRUE(
            parser_parse_declaration_assignment(parser)
    ) << "Error parsing <assignment rule";


}

TEST_F(ParserTestFixture, DeclarationAssigment2) {

    provider->setString("");

    EXPECT_TRUE(
            parser_parse_declaration_assignment(parser)
    ) << "Error parsing <assignment rule";


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


}

TEST_F(ParserTestFixture, ComplexTest4) {

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

TEST_F(ParserTestFixture, ComplexTestFalse) {
    provider->setString(R"(
SCOPE

inpu id
print 42;

END SCOPE
    )");
    EXPECT_FALSE(
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

TEST_F(ParserTestFixture, ComplexTest3) {
    provider->setString(R"(
declare function foo_function(a as integer, b as string) as string
function bar(b as string, a as integer) as integer
dim a as integer = 69887
dim Z6_7890 as integer = 6709890
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
elseif 43 then
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
elseif 43 then
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

}

TEST_F(ParserTestFixture, ComplexTestWithCondition1) {

    provider->setString(R"(
SCOPE
return 32
if 42 then
input id
elseif 43 then
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

TEST_F(ParserTestFixture, ComplexTestWithConditionWithoutElseif) {
    provider->setString(R"(
SCOPE
if 42 then
input id
ELSE
input id
input id
end if
END SCOPE
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

}

TEST_F(ParserTestFixture, IdentifAssignment) {
    provider->setString(R"(
SCOPE
if 42 then
input id
ELSE
input id
input id
end if
END SCOPE
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

}

TEST_F(ParserTestFixture, SimpleIf) {
    provider->setString(R"(

scope 'Hlavni telo programu


if 42 then


input id

end if


END SCOPE
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

}

TEST_F(ParserTestFixture, ComplexTestIdentifAssignement) {

    provider->setString("ahoj = 42");

    EXPECT_TRUE(
            parser_parse_identifier_assignment(parser)
    ) << "Body parse";

}

TEST_F(ParserTestFixture, ComplexTestFactorial) {
    provider->setString(R"(
/' Program 1: Vypocet faktorialu (iterativne) '/
/' Vcetne ukazky case-insensitive vlastnosti jazyka IFJ17 '/

scope 'Hlavni telo programu

Dim a As Integer
DIM vysl AS INTEGER

PrinT !"Zadejte cislo pro vypocet faktorialu";
InpuT A

If a < 0 THEN
print !"\nFaktorial nelze spocitat\n";

ELSE

Vysl = 1

Do WHile A > 0
VYSL = vysl * a
a = A - 1
LooP

Print !"\nVysledek je:" ; vYsl ; !"\n";
end IF


END SCOPE
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

}

TEST_F(ParserTestFixture, ComplexTestFactorialRecursive) {
    provider->setString(R"(
/' Program 2: Vypocet faktorialu (rekurzivne) '/

Declare Function factorial (n As Integer) As Integer

Function factorial (n As Integer) As Integer

Dim temp_result As Integer
Dim decremented_n As Integer
Dim result As Integer

If n < 2 Then
result = 1
Else
decremented_n = n - 1
temp_result = 42                                         'factorial(decremented_n)
result = n * temp_result
End If
Return result
End Function

Scope 'Hlavni telo programu
Dim a As Integer
Dim vysl As Integer

Print !"Zadejte cislo pro vypocet faktorialu";
Input a

If a < 0 Then
Print !"\nFaktorial nelze spocitat\n";
else
vysl = 42                                                'factorial(a)
Print !"\nVysledek je:" ; vysl ; !"\n";
End If

End Scope
    )");
    EXPECT_TRUE(
            parser_parse_program(parser)
    ) << "Body parse";

}







