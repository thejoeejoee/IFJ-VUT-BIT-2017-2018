#include "token.h"
#include "memory.h"
#include <string.h>

Token token_copy(Token token)
{
    if (token.data == NULL) {
        return token;
    } else {
        size_t len = strlen(token.data);
        char *data = memory_alloc(sizeof(char) * (len + 1));
        if (NULL == strcpy(data, token.data)) {
            exit_with_code(ERROR_INTERNAL);
        }
        Token tmp = { .data = data, .type = token.type };
        return tmp;
    }
}

void token_free(Token* token) {
    NULL_POINTER_CHECK(token,);
    if(token->data != NULL) {
        memory_free(token->data);
        token->data = NULL;
    }
}
