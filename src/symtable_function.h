#ifndef SYMTABLE_FUNCTION_H
#define SYMTABLE_FUNCTION_H

#include "common.h"
#include "symtable.h"
#include "data_type.h"

typedef struct symbol_function_param_t {
    char* name;
    DataType data_type;
    struct symbol_function_param_t* next;
} SymbolFunctionParam;

typedef struct symbol_function_t {
    SymbolTableBaseItem base;

    bool declared;
    bool defined;
    DataType return_data_type;
    int count_of_argument;
    SymbolFunctionParam* param;
} SymbolFunction;

SymbolFunction* symbol_table_function_get_or_create(SymbolTable* table, const char* key);

SymbolFunction* symbol_table_function_get(SymbolTable* table, const char* key);

void symbol_function_init_data(SymbolTableBaseItem* item);

void symbol_function_free_data(SymbolTableBaseItem* item);

SymbolFunctionParam* symbol_function_add_param(SymbolFunction* function, char* name, DataType data_type);

SymbolFunctionParam* symbol_function_get_param(SymbolFunction* function, size_t index);


#define symbol_table_function_init(bucket_count) symbol_table_init( \
    bucket_count, \
    sizeof(SymbolFunction), \
    symbol_function_init_data, \
    symbol_function_free_data \
)

#endif // SYMTABLE_FUNCTION_H
