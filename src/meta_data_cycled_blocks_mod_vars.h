#ifndef META_DATA_CYCLED_BLOCKS_MOD_VARS_H
#define META_DATA_CYCLED_BLOCKS_MOD_VARS_H

#include "llist.h"
#include "symtable.h"
#include "set_int.h"

typedef struct {
    LListBaseItem base;
    SetInt* blocks_ids;
    SymbolTable* mod_vars;
} MetaDataCycledBlocksModVars;

void meta_data_cycled_blocks_mod_vars_free(LListBaseItem* item);

#endif // META_DATA_CYCLED_BLOCKS_MOD_VARS_H
