#include "parser_expr.h"
#include "parser.h"
#include "token.h"
#include "llist.h"

typedef enum {
    // Terminals
    EXPR_TOKEN_UNARY_MINUS = 0,
    EXPR_TOKEN_MULTIPLY = 1,
    EXPR_TOKEN_DIVIDE = 2,
    EXPR_TOKEN_INTEGER_DIVIDE = 3,
    EXPR_TOKEN_PLUS = 4,
    EXPR_TOKEN_MINUS = 5,
    EXPR_TOKEN_EQUAL = 6,
    EXPR_TOKEN_NOT_EQUAL = 7,
    EXPR_TOKEN_LESSER = 8,
    EXPR_TOKEN_LESSER_OR_EQUAL = 9,
    EXPR_TOKEN_GREATHER_OR_EQUAL = 10,
    EXPR_TOKEN_GREATHER = 11,
    EXPR_TOKEN_NOT = 12,
    EXPR_TOKEN_AND = 13,
    EXPR_TOKEN_OR = 14,

    EXPR_TOKEN_LEFT_BRACKET = 15,
    EXPR_TOKEN_RIGHT_BRACKET = 16,

    EXPR_TOKEN_FUNCTION = 17,
    EXPR_TOKEN_IDENTIFIER = 18,
    EXPR_TOKEN_$ = 19,

    // Others
    EXPR_LEFT_SHARP, EXPR_SHIFT = EXPR_LEFT_SHARP,      // <
    EXPR_RIGHT_SHARP, EXPR_REDUCE = EXPR_RIGHT_SHARP,   // >
    EXPR_EXPRESSION,    // E

    EXPR_UNKNOWN
} ExprTokenType;

typedef unsigned ExprIdx;

typedef union expr_data_u {
    ExprIdx idx;
    char* s;
} ExprData;

typedef struct expr_token_t {
    ExprTokenType type;
    ExprData data;
} ExprToken;

ExprToken* load_expr_token(Lexer* lexer) {
    Token t = lexer_next_token(lexer);
    ExprToken* expr_t = memory_alloc(sizeof(ExprToken));

    switch (t.type)
    {
    case TOKEN_MULTIPLY:
        expr_t->type = EXPR_TOKEN_MULTIPLY;
        token_free(&t);
        break;
    case TOKEN_DIVIDE:
        expr_t->type = EXPR_TOKEN_DIVIDE;
        token_free(&t);
        break;
    case TOKEN_INTEGER_DIVIDE:
        expr_t->type = EXPR_TOKEN_INTEGER_DIVIDE;
        token_free(&t);
        break;
    case TOKEN_ADD:
        expr_t->type = EXPR_TOKEN_PLUS;
        token_free(&t);
        break;
    case TOKEN_SUBTRACT:
        expr_t->type = EXPR_TOKEN_MINUS;
        token_free(&t);
        break;
    case TOKEN_EQUAL:
        expr_t->type = EXPR_TOKEN_EQUAL;
        token_free(&t);
        break;
    case TOKEN_SMALLER_BIGGER:
        expr_t->type = EXPR_TOKEN_NOT_EQUAL;
        token_free(&t);
        break;
    case TOKEN_SMALLER:
        expr_t->type = EXPR_TOKEN_LESSER;
        token_free(&t);
        break;
    case TOKEN_SMALLER_EQUAL:
        expr_t->type = EXPR_TOKEN_LESSER_OR_EQUAL;
        token_free(&t);
        break;
    case TOKEN_BIGGER_EQUAL:
        expr_t->type = EXPR_TOKEN_GREATHER_OR_EQUAL;
        token_free(&t);
        break;
    case TOKEN_BIGGER:
        expr_t->type = EXPR_TOKEN_GREATHER;
        token_free(&t);
        break;
    case TOKEN_NOT:
        expr_t->type = EXPR_TOKEN_NOT;
        token_free(&t);
        break;
    case TOKEN_AND:
        expr_t->type = EXPR_TOKEN_AND;
        token_free(&t);
        break;
    case TOKEN_OR:
        expr_t->type = EXPR_TOKEN_OR;
        token_free(&t);
        break;
    case TOKEN_LEFT_BRACKET:
        expr_t->type = EXPR_TOKEN_LEFT_BRACKET;
        token_free(&t);
        break;
    case TOKEN_RIGHT_BRACKET:
        expr_t->type = EXPR_TOKEN_RIGHT_BRACKET;
        token_free(&t);
        break;
    case TOKEN_FUNCTION: // We steal token data here, and we free it later in expr_token_free.
        expr_t->type = EXPR_TOKEN_FUNCTION;
        expr_t->data.s = t.data; //TODO check function declaration
        break;
    case TOKEN_IDENTIFIER: // We steal token data here, and we free it later in expr_token_free.
        expr_t->type = EXPR_TOKEN_IDENTIFIER;
        expr_t->data.s = t.data; //TODO check identifier declaration
        break;
    default:
        expr_t->type = EXPR_UNKNOWN;
        lexer_rewind_token(lexer, t);
        token_free(&t);
        break;
    }

    return expr_t;
}

void expr_token_free(ExprToken *t) {
    if (t != NULL) {
        if ((t->type == EXPR_TOKEN_IDENTIFIER || t->type == EXPR_TOKEN_FUNCTION) && t->data.s != NULL) {
            memory_free(t->data.s);
        }
        memory_free(t);
    }
}

int expr_llist_type_cmp(void* a, void* b) {
    ASSERT(a != NULL && b != NULL);
    if (((ExprToken*)a)->type == ((ExprToken*)b)->type) {
        return 0;
    } else {
        return -1;
    }
}

void expr_llist_free(void* data) {
    ASSERT(data != NULL);
    expr_token_free((ExprToken*)data);
}

void expr_token_update_unary(ExprToken* minus, const ExprToken* previous) {
    //TODO: what about unary + ?
    ASSERT(minus->type == EXPR_TOKEN_MINUS);
    ExprTokenType prev_t = previous->type;
    if (prev_t != EXPR_EXPRESSION) {
        minus->type = EXPR_TOKEN_UNARY_MINUS;
    }
}

ExprToken* create_expr_token(ExprTokenType type) {
    ExprToken* t;
    t = memory_alloc(sizeof(ExprToken));
    t->type = type;
    return t;
}

ExprToken* create_expression(ExprIdx index) {
    ExprToken* t;
    t = memory_alloc(sizeof(ExprToken));
    t->type = EXPR_EXPRESSION;
    t->data.idx = index;
    return t;
}

ExprToken* expr_last_terminal(LList *expr_token_buffer) {
    LListItem* it = expr_token_buffer->tail;
    while (((ExprToken*)it->value)->type > EXPR_TOKEN_$ || ((ExprToken*)it->value)->type < 0) {
        it = it->previous;
    }
    return it->value;
}
void expr_llist_append_after_last_terminal(LList *expr_token_buffer, ExprToken* token) {
    LListItem* it = expr_token_buffer->tail;
    while (((ExprToken*)it->value)->type > EXPR_TOKEN_$ || ((ExprToken*)it->value)->type < 0) {
        it = it->previous;
        ASSERT(it != NULL);
    }
    llist_insert_after(expr_token_buffer, it, token);
}

ExprToken* expr_get_precedence(ExprToken *a, ExprToken *b) {
    ExprToken* r = create_expr_token(EXPR_UNKNOWN);



    return r;
}

bool expression_reduce(Parser* parser, LList *expr_token_buffer, ExprIdx* expression_idx) {
    return false;

    //example: some rule
    create_expression(*expression_idx);
    (*expression_idx)++;

    return true;
}

bool parser_parse_expression(Parser* parser) {
    ExprToken *token, *precedence;
    ExprIdx expression_idx = 0; //TODO: maybe should be static in expression_reduce, because of function calls
    LList *buffer;
    llist_init(&buffer, expr_llist_free, expr_llist_type_cmp);

    llist_append(buffer, create_expr_token(EXPR_TOKEN_$));

    do { //TODO: check end of expression from token and prec, rewind token (rewrite load_expr_token)
        token = load_expr_token(parser->lexer);
        if (token->type == EXPR_TOKEN_MINUS) {
            expr_token_update_unary(token, buffer->tail->value); // check if minus is unary and update
        }

        do {
            precedence = expr_get_precedence(expr_last_terminal(buffer), token);
            expr_llist_append_after_last_terminal(buffer, precedence);
            if (precedence->type == EXPR_REDUCE) {
                if (!expression_reduce(parser, buffer, &expression_idx)) {
                    return false; //expression reduction - no rule has been found
                }
            }
        } while (precedence->type == EXPR_REDUCE);
        llist_append(buffer, token);

    } while (token->type != EXPR_UNKNOWN);

    llist_free(buffer);
    return true;
}