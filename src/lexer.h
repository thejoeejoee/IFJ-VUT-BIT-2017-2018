#ifndef _LEXER_H
#define _LEXER_H

#include "token.h"


typedef int (* lexer_input_stream_f)();

typedef struct lexer_t {
    lexer_input_stream_f input_stream;
} Lexer;


Lexer* lexer_init(lexer_input_stream_f);

Token *lexer_next_token(Lexer* lexer);

#endif //_LEXER_H
