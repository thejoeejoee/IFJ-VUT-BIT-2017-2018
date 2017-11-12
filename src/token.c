#include "token.h"
#include "memory.h"
#include <string.h>

bool token_check(Token token, TokenType type) {
    return !(((token.type & type) == 0 && type >= TOKEN_CLASSES && (type & 0xFF) == 0) ||
             (token.type != type && type < TOKEN_CLASSES));
}

Token token_copy(Token token) {
    if(token.data == NULL) {
        return token;
    }
    Token tmp = {
            .data = c_string_copy(token.data),
            .type = token.type
    };
    return tmp;
}

void token_free(Token* token) {
    NULL_POINTER_CHECK(token,);
    if(token->data != NULL) {
        memory_free(token->data);
        token->data = NULL;
    }
}
