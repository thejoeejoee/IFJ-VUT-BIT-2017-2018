#include "meta_data_cycled_blocks_mod_vars.h"

void meta_data_cycled_blocks_mod_vars_free(LListBaseItem* item)
{
    NULL_POINTER_CHECK(item, );

    MetaDataCycledBlocksModVars* v = (MetaDataCycledBlocksModVars*) item;
    symbol_table_free(v->mod_vars);
    set_int_free(&v->blocks_ids);
}
