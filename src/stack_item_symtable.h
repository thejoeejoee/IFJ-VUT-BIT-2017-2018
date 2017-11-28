#ifndef STACK_ITEM_SYMTABLE_H
#define STACK_ITEM_SYMTABLE_H

#include "symtable.h"
#include "stack.h"

typedef struct {
    StackBaseItem base;
    SymbolTable* symtable;
} StackItemSymtable;

StackItemSymtable* init_data_stack_item_symtable(SymbolTable* table);
void free_data_stack_item_symtable(StackBaseItem* item);

#endif // STACK_ITEM_SYMTABLE_H
