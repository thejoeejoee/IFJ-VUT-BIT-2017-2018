#include "meta_data_mapped_operand.h"

void init_mapped_operand_item(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);

    MappedOperand* v = (MappedOperand*) item;
    v->operand = NULL;
    v->blocked = false;
}

void free_mapped_operand_item(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);

    MappedOperand* v = (MappedOperand*) item;
    if(v->operand != NULL)
        code_instruction_operand_free(&v->operand);
}

void copy_mapped_operand_item(SymbolTableBaseItem* to, SymbolTableBaseItem* from)
{
    MappedOperand* v_to = (MappedOperand*) to;
    MappedOperand* v_from = (MappedOperand*) from;

    v_to->operand = code_instruction_operand_copy(v_from->operand);
    v_to->blocked = v_from->blocked;
}
