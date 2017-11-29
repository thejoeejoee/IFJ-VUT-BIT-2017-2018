#include "meta_data_constants_tables_stack.h"

ConstantsTableStackItem* constants_table_stack_item_init(SymbolTable* constants)
{
    ConstantsTableStackItem* new_item = memory_alloc(sizeof(ConstantsTableStackItem));
    new_item->is_direct_table = true;
    if(constants == NULL) {
        new_item->constants = symbol_table_init(32, sizeof(MappedOperand),
                                                &init_mapped_operand_item,
                                                &free_mapped_operand_item);
    }

    else {
        new_item->constants = constants;
    }

    return new_item;
}

void constants_table_stack_item_free(StackBaseItem* item)
{
    NULL_POINTER_CHECK(item, );

    ConstantsTableStackItem* v = (ConstantsTableStackItem*) item;
    symbol_table_free(v->constants);
}

ConstantsTableStackItem* constants_tables_stack_get_lowest_direct_table(Stack* stack)
{
    NULL_POINTER_CHECK(stack, NULL);

    ConstantsTableStackItem* item = (ConstantsTableStackItem*) stack->head;
    while(item != NULL) {
        if(item->is_direct_table)
            return item;
        item = (ConstantsTableStackItem*) item->base.next;
    }

    LOG_WARNING("Internal error no direct table found.");
    return NULL;
}
