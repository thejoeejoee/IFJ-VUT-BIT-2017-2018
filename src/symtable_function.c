#include "symtable_function.h"

SymbolFunction* symbol_table_function_get_or_create(SymbolTable* table, const char* key)
{
    return (SymbolFunction*)symbol_table_get_or_create(table, key);
}

SymbolFunction* symbol_table_function_get(SymbolTable* table, const char* key)
{
    return (SymbolFunction*)symbol_table_get(table, key);
}

void symbol_function_free_data(SymbolTableBaseItem* item) {
    (void)item;
}

void symbol_function_init_data(SymbolTableBaseItem *item) {
    (void)item;
}
