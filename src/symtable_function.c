#include "symtable_function.h"

SymbolFunction* symbol_table_function_get_or_create(SymbolTable* table, const char* key) {
    SymbolFunction* symbol_function_return = (SymbolFunction*) symbol_table_get_or_create(table, key);
    return symbol_function_return;
}

SymbolFunction* symbol_table_function_get(SymbolTable* table, const char* key) {
    return (SymbolFunction*) symbol_table_get(table, key);
}

void symbol_function_free_data(SymbolTableBaseItem* item) {
    (void) item;
}

void symbol_function_init_data(SymbolTableBaseItem* item) {
    SymbolFunction* function = (SymbolFunction*) item;

    function->return_data_type = DATA_TYPE_NONE;
    function->declared = false;
    function->defined = false;
}
