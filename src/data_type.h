#ifndef _DATA_TYPE_H
#define _DATA_TYPE_H

#include "token.h"

typedef enum {
    DATA_TYPE_NONE = 0,
    DATA_TYPE_INTEGER = TOKEN_INTEGER,
    DATA_TYPE_DOUBLE = TOKEN_DOUBLE,
    DATA_TYPE_STRING = TOKEN_STRING,
    DATA_TYPE_BOOLEAN, // TODO: assign to one of tokens
} DataType;

#endif //_DATA_TYPE_H
