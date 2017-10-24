#include "parser_expr.h"
#include "parser_expr_internal.h"
#include "parser_expr_rules.h"
#include "parser.h"
#include "llist.h"

bool expression_reduce(Parser* parser, LList* expr_token_buffer, ExprIdx* expression_idx) {
    bool pass = false;
    const int table_size = sizeof(expr_rule_table) / sizeof(*expr_rule_table);
    for(int i = 0; i < table_size; i++) {
        pass |= expr_rule_table[i](parser, expr_token_buffer, expression_idx);
        if(
                parser->parser_semantic->error_report.error_code != ERROR_NONE
                &&
                parser->run_type & PARSER_RUN_TYPE_SEMANTIC_ANALYSIS
                ) {
            return false;
        }
        if(pass == true) { break; }
    }
    return pass;
}

bool parser_parse_expression(Parser* parser) {
    Token last_token;
    last_token.data = NULL;
    ExprToken* token, * precedence;
    ExprIdx expression_idx = 0; //TODO: maybe should be static in expression_reduce, because of function calls
    LList* buffer;
    llist_init(&buffer, sizeof(ExprToken), NULL, expr_llist_free, expr_llist_type_cmp);

    token = (ExprToken*) llist_new_tail_item(buffer);
    token->type = EXPR_TOKEN_$;

    do {
        token = load_expr_token(parser->lexer, &last_token);
        if(token->type == EXPR_TOKEN_MINUS) {
            expr_token_update_unary(token, (ExprToken*) buffer->tail); // check if minus is unary and update
        }

        do {
            precedence = expr_get_precedence(expr_last_terminal(buffer), token);

            // Check for delimiter of expression
            if(token->type != EXPR_TOKEN_$ && precedence->type == EXPR_UNKNOWN) {
                lexer_rewind_token(parser->lexer, last_token);
                expr_token_free(token);
                expr_token_free(precedence);
                token = create_expr_token(EXPR_TOKEN_$);
                precedence = expr_get_precedence(expr_last_terminal(buffer), token);
            }

            // Check completion of expression parsing
            if(is_expr_parsing_complete(buffer, token)) {
                expr_token_free(token);
                expr_token_free(precedence);
                // Cleanup
                token_free(&last_token);
                llist_free(&buffer);
                return true;
            }

            if(precedence->type == EXPR_LEFT_SHARP) {
                expr_llist_append_after_last_terminal(buffer, precedence);
            } else if(precedence->type == EXPR_RIGHT_SHARP) {
                llist_append_item(buffer, (LListBaseItem*) precedence);
            } else if(precedence->type == EXPR_SAME) {
                expr_token_free(precedence);
            } else {
                expr_token_free(token);
                expr_token_free(precedence);
                // Cleanup
                token_free(&last_token);
                llist_free(&buffer);
                return false; // Precedence error - undefined
            }

            if(((ExprToken*) buffer->tail)->type == EXPR_REDUCE) {
                if(expression_reduce(parser, buffer, &expression_idx)) {
                    continue;
                } else {
                    expr_token_free(token);
                    // Cleanup
                    token_free(&last_token);
                    llist_free(&buffer);
                    return false; // Expression reduction error - no rule has been found
                }
            }
            if(((ExprToken*) buffer->tail)->type != EXPR_REDUCE) {
                break;
            }
        } while(true);
        llist_append_item(buffer, (LListBaseItem*) token);

    } while(token->type != EXPR_TOKEN_$);

    // Cleanup
    token_free(&last_token);
    llist_free(&buffer);
    return false; //Expression procession error
}
