#ifndef META_DATA_CONSTANTS_TABLES_STACK_H
#define META_DATA_CONSTANTS_TABLES_STACK_H

#include "stack.h"
#include "symtable.h"
#include "meta_data_mapped_operand.h"

typedef struct {
    StackBaseItem base;
    SymbolTable* constants;
    bool is_direct_table;
} ConstantsTableStackItem;

ConstantsTableStackItem* constants_table_stack_item_init(SymbolTable* constants);
void constants_table_stack_item_free(StackBaseItem* item);

ConstantsTableStackItem* constants_tables_stack_get_lowest_direct_table(Stack* stack);

#endif // META_DATA_CONSTANTS_TABLES_STACK_H
