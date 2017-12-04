#ifndef META_DATA_MAPPED_OPERAND_H
#define META_DATA_MAPPED_OPERAND_H

#include "symtable.h"
#include "code_instruction.h"
#include "code_instruction_operand.h"

typedef struct mapped_operand_t {
    SymbolTableBaseItem base;
    CodeInstructionOperand* operand;
    bool blocked;
    CodeInstruction* setter;
} MappedOperand;

void init_mapped_operand_item(SymbolTableBaseItem* item);
void free_mapped_operand_item(SymbolTableBaseItem* item);
void copy_mapped_operand_item(SymbolTableBaseItem* to, SymbolTableBaseItem* from);

#endif // META_DATA_MAPPED_OPERAND_H
