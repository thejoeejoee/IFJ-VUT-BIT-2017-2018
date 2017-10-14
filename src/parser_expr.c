#include "parser_expr.h"
#include "parser_expr_internal.h"
#include "parser.h"
#include "llist.h"


bool expression_reduce(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx) {
    if (((ExprToken*)expr_token_buffer->tail->value)->type != EXPR_REDUCE) {
        return false;
    }

    // ***FAKE Reduction***
    ExprToken *tmp;
    ExprTokenType type;
    do {
        tmp = llist_pop_back(expr_token_buffer);
        if (tmp == NULL) {
            return false;
        }
        type = tmp->type;
        expr_token_free(tmp);
    } while (type != EXPR_LEFT_SHARP);
    return true;

    //example: some rule that generates E
    /*
    ExprToken* e = create_expression(*expression_idx);
    (*expression_idx)++;
    llist_append(expr_token_buffer, e);
    return true;
    */
}

bool parser_parse_expression(Parser* parser) {
    Token last_token;
    last_token.data = NULL;
    ExprToken *token, *precedence;
    ExprIdx expression_idx = 0; //TODO: maybe should be static in expression_reduce, because of function calls
    LList *buffer;
    llist_init(&buffer, expr_llist_free, expr_llist_type_cmp);

    llist_append(buffer, create_expr_token(EXPR_TOKEN_$));

    do {
        token = load_expr_token(parser->lexer, &last_token);
        if (token->type == EXPR_TOKEN_MINUS) {
            expr_token_update_unary(token, buffer->tail->value); // check if minus is unary and update
        }

        do {
            precedence = expr_get_precedence(expr_last_terminal(buffer), token);
            
            // Check for end of expression
            if (token->type != EXPR_TOKEN_$ && precedence->type == EXPR_UNKNOWN) {
                lexer_rewind_token(parser->lexer, last_token);
                expr_token_free(token);
                expr_token_free(precedence);
                token = create_expr_token(EXPR_TOKEN_$);
                precedence = expr_get_precedence(expr_last_terminal(buffer), token);
            }

            // Check for function call
            if (precedence->type == EXPR_TOKENCHANGE) {
                ExprToken* back = buffer->tail->value;
                if (back->type == EXPR_TOKEN_IDENTIFIER && token->type == EXPR_TOKEN_LEFT_BRACKET) {
                    back->type = EXPR_TOKEN_FUNCTION_CALL;
                    expr_token_free(precedence);
                    expr_token_free(token);
                    token = llist_pop_back(buffer);
                    break;
                } else {
                    // Cleanup
                    token_free(&last_token);
                    llist_free(&buffer);
                    return false; // Expression error - unknown token change transition
                }
            }

            if (precedence->type != EXPR_SAME) {
                expr_llist_append_after_last_terminal(buffer, precedence);
            } else {
                expr_token_free(precedence);
            }

            if (((ExprToken*)buffer->tail->value)->type == EXPR_REDUCE) {
                if (!expression_reduce(parser, buffer, &expression_idx)) {
                    // Cleanup
                    token_free(&last_token);
                    llist_free(&buffer);
                    return false; // Expression reduction error - no rule has been found
                }
            }
        } while (((ExprToken*)buffer->tail->value)->type == EXPR_REDUCE);
        llist_append(buffer, token);

    } while (token->type != EXPR_TOKEN_$);

    // Cleanup
    token_free(&last_token);
    llist_free(&buffer);
    return true;
}