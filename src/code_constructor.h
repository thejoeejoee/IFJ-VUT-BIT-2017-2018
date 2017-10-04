#ifndef _CODE_CONSTRUCTOR_H
#define _CODE_CONSTRUCTOR_H

#include "code_generator.h"

typedef struct code_constructor_t {
    CodeGenerator* generator;
} CodeConstructor;

CodeConstructor* code_constructor_init();

void code_constructor_free(CodeConstructor** constructor);

#endif //_CODE_CONSTRUCTOR_H
