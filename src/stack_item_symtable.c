#include "stack_item_symtable.h"

void free_data_stack_item_symtable(StackBaseItem* item)
{
    StackItemSymtable* v = (StackItemSymtable*) item;
    symbol_table_free(v->symtable);
}

StackItemSymtable* init_data_stack_item_symtable(SymbolTable* table)
{
    NULL_POINTER_CHECK(table, NULL);

    StackItemSymtable* new_item = memory_alloc(sizeof(StackItemSymtable*));
    new_item = symbol_table_copy(table);

    return new_item;
}
