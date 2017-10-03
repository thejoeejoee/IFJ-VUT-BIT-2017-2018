#include "token.h"

void token_free(Token* token) {
    NULL_POINTER_CHECK(token,);
    if(token->data != NULL) {
        memory_free(token->data);
        token->data = NULL;
    }
}
