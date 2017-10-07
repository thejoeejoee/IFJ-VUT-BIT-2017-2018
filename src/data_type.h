#ifndef _DATA_TYPE_H
#define _DATA_TYPE_H

#include "token.h"

typedef enum {
    DATA_TYPE_INTEGER = TOKEN_INTEGER_LITERAL,
    DATA_TYPE_DOUBLE = TOKEN_DOUBLE_LITERAL,
    DATA_TYPE_STRING = TOKEN_STRING_VALUE,
    DATA_TYPE_BOOLEAN, // TODO: assign to one of tokens
} DataType;

#endif //_DATA_TYPE_H
