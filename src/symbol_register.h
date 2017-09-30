#ifndef _SYMBOL_REGISTER_H
#define _SYMBOL_REGISTER_H

#include "symtable.h"

typedef struct symbol_register_t {
    SymbolTableSymbolFunction* functions;
    SymbolTableSymbolVariable* variables;
} SymbolRegister;

SymbolRegister* symbol_register_init();

void symbol_register_free(SymbolRegister** register_);

void symbol_register_push_variables_table(SymbolRegister* register_);

void symbol_register_pop_variables_table(SymbolRegister* register_);

SymbolVariable symbol_register_find_variable(SymbolRegister* register_, const char* key);

SymbolVariable symbol_register_find_variable_recursive(SymbolRegister* register_, const char* key);

#endif //_SYMBOL_REGISTER_H
