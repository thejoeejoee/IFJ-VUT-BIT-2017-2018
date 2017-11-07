#include "token.h"
#include "memory.h"
#include <string.h>

Token token_copy(Token token) {
    if(token.data == NULL) {
        return token;
    }

    return {
            .data = c_string_copy(token.data),
            .type = token.type
    };
}

void token_free(Token* token) {
    NULL_POINTER_CHECK(token,);
    if(token->data != NULL) {
        memory_free(token->data);
        token->data = NULL;
    }
}
