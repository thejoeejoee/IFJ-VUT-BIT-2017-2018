#include "parser_expr_internal.h"
#include "lexer.h"
#include "token.h"
#include "llist.h"
#include "common.h"

static ExprTokenType _expr_internal_fn_literal_to_id(ExprTokenType t) {
    if(t >= EXPR_TOKEN_BOOLEAN_LITERAL &&
       t <= EXPR_TOKEN_FN_CHR) {
        t = EXPR_TOKEN_IDENTIFIER;
    }
    return t;
}

static ExprTokenType _expr_get_precedence(ExprTokenType a, ExprTokenType b) {
    ExprTokenType t;
    if(!(a & EXPR_TERMINALS_MASK)) { return EXPR_UNKNOWN; }
    if(!(b & EXPR_TERMINALS_MASK)) { return EXPR_UNKNOWN; }

    a = _expr_internal_fn_literal_to_id(a); // treat literals precedence as same as identifiers
    b = _expr_internal_fn_literal_to_id(b); // treat literals precedence as same as identifiers

    a--;
    b--;
    char precedence = prec_table[((a * prec_table_size) * 2) + (b * 2)];
    switch(precedence) {
        case '<':
            t = EXPR_LEFT_SHARP;
            break;
        case '>':
            t = EXPR_RIGHT_SHARP;
            break;
        case '=':
            t = EXPR_SAME;
            break;
        case '*':
            t = EXPR_TOKEN_CHANGE;
            break;
        case 'x':
        case '#':
        case ' ':
        default:
            t = EXPR_UNKNOWN;
            break;
    }
    return t;
}

ExprToken* expr_get_precedence(ExprToken* a, ExprToken* b) {
    ExprToken* r = create_expr_token(EXPR_UNKNOWN);
    r->type = _expr_get_precedence(a->type, b->type);
    return r;
}

bool is_expr_parsing_complete(LList* expr_token_buffer, ExprToken* token) {
    LListBaseItem* tmp = expr_token_buffer->head;
    return ((ExprToken*) tmp)->type == EXPR_TOKEN_$ &&
           (tmp = tmp->next) != NULL &&
           ((ExprToken*) tmp)->type == EXPR_EXPRESSION &&
           tmp->next == NULL &&
           token->type == EXPR_TOKEN_$;
}

ExprToken* load_expr_token(Lexer* lexer, Token* last_token) {
    NULL_POINTER_CHECK(lexer, NULL);
    NULL_POINTER_CHECK(last_token, NULL);

    token_free(last_token);

    *last_token = lexer_next_token(lexer);
    ExprToken* expr_t = expr_token_init();
    expr_t->data.s = NULL;

    switch(last_token->type) {
        case TOKEN_MULTIPLY:
            expr_t->type = EXPR_TOKEN_MULTIPLY;
            break;
        case TOKEN_DIVIDE:
            expr_t->type = EXPR_TOKEN_DIVIDE;
            break;
        case TOKEN_INTEGER_DIVIDE:
            expr_t->type = EXPR_TOKEN_INTEGER_DIVIDE;
            break;
        case TOKEN_ADD:
            expr_t->type = EXPR_TOKEN_PLUS;
            break;
        case TOKEN_SUBTRACT:
            expr_t->type = EXPR_TOKEN_MINUS;
            break;
        case TOKEN_EQUAL:
            expr_t->type = EXPR_TOKEN_EQUAL;
            break;
        case TOKEN_SMALLER_BIGGER:
            expr_t->type = EXPR_TOKEN_NOT_EQUAL;
            break;
        case TOKEN_SMALLER:
            expr_t->type = EXPR_TOKEN_LESSER;
            break;
        case TOKEN_SMALLER_EQUAL:
            expr_t->type = EXPR_TOKEN_LESSER_OR_EQUAL;
            break;
        case TOKEN_BIGGER_EQUAL:
            expr_t->type = EXPR_TOKEN_GREATHER_OR_EQUAL;
            break;
        case TOKEN_BIGGER:
            expr_t->type = EXPR_TOKEN_GREATHER;
            break;
        case TOKEN_NOT:
            expr_t->type = EXPR_TOKEN_NOT;
            break;
        case TOKEN_AND:
            expr_t->type = EXPR_TOKEN_AND;
            break;
        case TOKEN_OR:
            expr_t->type = EXPR_TOKEN_OR;
            break;
        case TOKEN_LEFT_BRACKET:
            expr_t->type = EXPR_TOKEN_LEFT_BRACKET;
            break;
        case TOKEN_RIGHT_BRACKET:
            expr_t->type = EXPR_TOKEN_RIGHT_BRACKET;
            break;
        case TOKEN_COMMA:
            expr_t->type = EXPR_TOKEN_COMMA;
            break;
        case TOKEN_IDENTIFIER:
            expr_t->type = EXPR_TOKEN_IDENTIFIER;
            expr_t->data.s = c_string_copy(last_token->data);
            break;
            // Literals
        case TOKEN_TRUE:
            expr_t->type = EXPR_TOKEN_BOOLEAN_LITERAL;
            expr_t->data.b = true;
            break;
        case TOKEN_FALSE:
            expr_t->type = EXPR_TOKEN_BOOLEAN_LITERAL;
            expr_t->data.b = false;
            break;
        case TOKEN_DOUBLE_LITERAL:
            expr_t->type = EXPR_TOKEN_DOUBLE_LITERAL;
            expr_t->data.s = c_string_copy(last_token->data);
            break;
        case TOKEN_INTEGER_LITERAL:
            expr_t->type = EXPR_TOKEN_INTEGER_LITERAL;
            expr_t->data.s = c_string_copy(last_token->data);
            break;
        case TOKEN_STRING_VALUE:
            expr_t->type = EXPR_TOKEN_STRING_LITERAL;
            expr_t->data.s = c_string_copy(last_token->data);
            break;
            // Internal functions
        case TOKEN_LENGTH:
            expr_t->type = EXPR_TOKEN_FN_LENGTH;
            break;
        case TOKEN_SUBSTR:
            expr_t->type = EXPR_TOKEN_FN_SUBSTR;
            break;
        case TOKEN_ASC:
            expr_t->type = EXPR_TOKEN_FN_ASC;
            break;
        case TOKEN_CHR:
            expr_t->type = EXPR_TOKEN_FN_CHR;
            break;
        default:
            expr_t->type = EXPR_UNKNOWN;
            break;
    }

    return expr_t;
}

void expr_token_free(ExprToken* t) {
    if(t != NULL) {
        if((t->type == EXPR_TOKEN_IDENTIFIER ||
            t->type == EXPR_TOKEN_INTEGER_LITERAL ||
            t->type == EXPR_TOKEN_DOUBLE_LITERAL ||
            t->type == EXPR_TOKEN_STRING_LITERAL
           ) && t->data.s != NULL) {
            memory_free(t->data.s);
        }
//         TODO check free
        memory_free(t);
    }
}


int expr_llist_type_cmp(LListBaseItem* a, LListBaseItem* b) {
    ASSERT(a != NULL && b != NULL);
    if(((ExprToken*) a)->type == ((ExprToken*) b)->type) {
        return 0;
    } else {
        return -1;
    }
}

void expr_llist_free(LListBaseItem* item) {
    ASSERT(item != NULL);
    ExprToken* t = (ExprToken*) item;
    if(t != NULL) {
        if((t->type == EXPR_TOKEN_IDENTIFIER ||
            t->type == EXPR_TOKEN_INTEGER_LITERAL ||
            t->type == EXPR_TOKEN_DOUBLE_LITERAL ||
            t->type == EXPR_TOKEN_STRING_LITERAL
           ) && t->data.s != NULL) {
            memory_free(t->data.s);
        }
    }
}

ExprToken* create_expr_token(ExprTokenType type) {
    ExprToken* t = expr_token_init();
    t->type = type;
    return t;
}

ExprToken* create_expression(ExprIdx index) {
    ExprToken* t = memory_alloc(sizeof(ExprToken));
    t->type = EXPR_EXPRESSION;
    t->data.idx = index;
    t->is_constant = false;
    t->instruction = NULL;
    t->data_type = DATA_TYPE_NONE;
    return t;
}

ExprToken* expr_last_terminal(LList* expr_token_buffer) {
    LListBaseItem* it = expr_token_buffer->tail;
    while(!(((ExprToken*) it)->type & EXPR_TERMINALS_MASK)) {
        it = it->previous;
    }
    return (ExprToken*) it;
}

void expr_llist_append_after_last_terminal(LList* expr_token_buffer, ExprToken* token) {
    LListBaseItem* it = expr_token_buffer->tail;
    while(!(((ExprToken*) it)->type & EXPR_TERMINALS_MASK)) {
        it = it->previous;
        ASSERT(it != NULL);
    }
    llist_insert_after(expr_token_buffer, it, (LListBaseItem*) token);
}

void expr_token_update_unary(ExprToken* minus, const ExprToken* previous) {
    //TODO: what about unary + ?
    ASSERT(minus->type == EXPR_TOKEN_MINUS);
    ExprTokenType prev_t = previous->type;
    if(
            prev_t != EXPR_EXPRESSION &&
            prev_t != EXPR_TOKEN_IDENTIFIER &&
            prev_t != EXPR_TOKEN_BOOLEAN_LITERAL &&
            prev_t != EXPR_TOKEN_INTEGER_LITERAL &&
            prev_t != EXPR_TOKEN_DOUBLE_LITERAL &&
            prev_t != EXPR_TOKEN_STRING_LITERAL &&
            prev_t != EXPR_TOKEN_RIGHT_BRACKET
            ) {
        minus->type = EXPR_TOKEN_UNARY_MINUS;
    }
}

void expr_replace(LList* expr_token_buffer, LListBaseItem* expr_token_buffer_left_sharp, ExprToken* single_expression) {
    LListBaseItem* i = expr_token_buffer_left_sharp;
    while((i = llist_remove_item(expr_token_buffer, i)) != NULL);
    llist_append_item(expr_token_buffer, (LListBaseItem*) single_expression);
}

ExprToken* get_next_expr(LListBaseItem** expr_token_buffer_item) {
    return ((ExprToken*) (get_next_expr_item(expr_token_buffer_item)));
}

LListBaseItem* get_next_expr_item(LListBaseItem** expr_token_buffer_item) {
    while(((ExprToken*) ((*expr_token_buffer_item)))->type != EXPR_EXPRESSION) {
        (*expr_token_buffer_item) = (*expr_token_buffer_item)->next;
        ASSERT((*expr_token_buffer_item) != NULL);
    }
    return (*expr_token_buffer_item);
}

ExprToken* expr_token_init() {
    ExprToken* et = memory_alloc(sizeof(ExprToken));
    et->base.next = NULL;
    et->base.previous = NULL;

    return et;
}

ExprToken* get_n_expr(LList* expression_list, size_t n) {
    NULL_POINTER_CHECK(expression_list, NULL);
    return ((ExprToken*) llist_get_n_from_end(expression_list, n));
}

ExprToken* expr_token_copy(ExprToken* t) {
    NULL_POINTER_CHECK(t, NULL);
    ExprToken* new = create_expr_token(t->type);
    new->is_constant = t->is_constant;
    new->instruction = t->instruction;
    new->data_type = t->data_type;
    new->base.next = new->base.previous = NULL;
    new->data.s = NULL;
    return new;
}
