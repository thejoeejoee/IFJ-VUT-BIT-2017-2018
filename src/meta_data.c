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
}

void init_label_meta_data(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);

    LabelMetaData* v = (LabelMetaData*) item;
    v->occurrences_count = 0;
}
