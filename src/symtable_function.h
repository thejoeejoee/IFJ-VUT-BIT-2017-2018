#ifndef SYMTABLE_FUNCTION_H
#define SYMTABLE_FUNCTION_H

#include "symtable.h"

typedef struct symbol_function_t {
    SymbolTableBaseItem base;
    // TODO: define all needed members
    bool declared;
    bool defined;
    short return_data_type;
} SymbolFunction;

SymbolFunction* symbol_table_function_get_or_create(SymbolTable* table, const char* key);
SymbolFunction* symbol_table_function_get(SymbolTable* table, const char* key);

void symbol_function_init_data(SymbolTableBaseItem* item);
void symbol_function_free_data(SymbolTableBaseItem* item);

#define symbol_table_function_init(bucket_count) symbol_table_init( \
    bucket_count, \
    sizeof(SymbolFunction), \
    symbol_function_init_data, \
    symbol_function_free_data)

#endif // SYMTABLE_FUNCTION_H
