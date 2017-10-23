#ifndef SYMTABLE_VARIABLE_H
#define SYMTABLE_VARIABLE_H

#include "symtable.h"
#include "data_type.h"

typedef enum {
    VARIABLE_FRAME_NONE,
    VARIABLE_FRAME_LOCAL,
    VARIABLE_FRAME_GLOBAL,
    VARIABLE_FRAME_TEMP,
} SymbolVariableFrame;

typedef struct symbol_variable_t {
    SymbolTableBaseItem base;

    DataType data_type;
    SymbolVariableFrame frame;
    size_t scope_depth;
} SymbolVariable;

/**
 * @internal
 */
SymbolVariable* symbol_table_variable_get_or_create(SymbolTable* table, const char* key);

SymbolVariable* symbol_table_variable_get(SymbolTable* table, const char* key);

void symbol_variable_init_data(SymbolTableBaseItem* item);

void symbol_variable_free_data(SymbolTableBaseItem* item);

SymbolVariable* symbol_variable_copy(SymbolVariable* variable);

#define symbol_table_variable_init(bucket_count) symbol_table_init( \
    bucket_count, \
    sizeof(SymbolVariable), \
    symbol_variable_init_data, \
    symbol_variable_free_data)

#endif // SYMTABLE_VARIABLE_H

