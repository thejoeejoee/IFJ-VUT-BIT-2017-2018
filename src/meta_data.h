#ifndef META_DATA_H
#define META_DATA_H

#include "symtable.h"

typedef enum {
    META_TYPE_PURE = 0,
    META_TYPE_DYNAMIC_DEPENDENT = 1,
    META_TYPE_OUTPUTED = 2,
    META_TYPE_WITH_SIDE_EFFECT = 4,
    META_TYPE_IMPURE = META_TYPE_OUTPUTED | META_TYPE_DYNAMIC_DEPENDENT | META_TYPE_WITH_SIDE_EFFECT
} MetaType;

typedef struct variable_meta_data_t {
    SymbolTableBaseItem base;
    int occurrences_count;
    MetaType purity_type;

    size_t read_usage_count;
} VariableMetaData;

typedef struct function_meta_data_t {
    SymbolTableBaseItem base;
    unsigned int call_count;
    MetaType purity_type;

    // has read
    // has write
    // has global mod
} FunctionMetaData;

typedef struct label_meta_data_t {
    SymbolTableBaseItem base;
    int occurrences_count;
} LabelMetaData;

// meta data sub item
void init_variable_meta_data(SymbolTableBaseItem* item);

void init_function_meta_data(SymbolTableBaseItem* item);

void init_label_meta_data(SymbolTableBaseItem* item);

#endif // META_DATA_H
