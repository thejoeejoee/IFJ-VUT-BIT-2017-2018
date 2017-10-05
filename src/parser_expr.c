#include "parser_expr.h"
#include "parser.h"
#include "token.h"

/*
 * Expressions will be parsed use precedent syntactic analysis.
 */

bool parser_parse_expression(Parser* parser) {


    // !!! I take the number as an expression.
    // !!! It will be replaced by parsing a complete string. I need it in other rules
    RULES(

            CHECK_TOKEN(TOKEN_INTEGER_LITERAL);
    );

    return true;
}