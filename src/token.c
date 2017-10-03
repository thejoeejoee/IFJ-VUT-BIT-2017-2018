#include "token.h"

void token_free(Token *token)
{
    if (token->data != NULL) {
        memory_free(token->data);
        token->data = NULL;
    }
}
