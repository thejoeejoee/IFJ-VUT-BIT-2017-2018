#include "meta_data.h"

void init_variable_meta_data(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);

    VariableMetaData* v = (VariableMetaData*) item;
    v->occurrences_count = 0;
    v->purity_type = META_TYPE_PURE;
    v->read_usage_count = 0;
}


void init_function_meta_data(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);

    FunctionMetaData* v = (FunctionMetaData*) item;
    v->call_count = 0;
    v->purity_type = META_TYPE_PURE;
    v->mod_global_vars = symbol_table_init(SYMBOL_TABLE_BASE_SIZE, sizeof(SymbolTableBaseItem), NULL, NULL);
    v->read_global_vars = symbol_table_init(SYMBOL_TABLE_BASE_SIZE, sizeof(SymbolTableBaseItem), NULL, NULL);
}

void free_function_meta_data(SymbolTableBaseItem* item)
{
    NULL_POINTER_CHECK(item,);

    FunctionMetaData* v = (FunctionMetaData*) item;
    symbol_table_free(v->mod_global_vars);
    symbol_table_free(v->read_global_vars);
    v->mod_global_vars = NULL;
    v->read_global_vars = NULL;
}

void init_label_meta_data(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);

    LabelMetaData* v = (LabelMetaData*) item;
    v->occurrences_count = 0;
}
