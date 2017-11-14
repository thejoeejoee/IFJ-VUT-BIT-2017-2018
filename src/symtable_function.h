#ifndef _SYMTABLE_FUNCTION_H
#define _SYMTABLE_FUNCTION_H

#include "common.h"
#include "symtable.h"
#include "data_type.h"
#include "dynamic_string.h"

#define symbol_table_function_init(bucket_count) symbol_table_init( \
    bucket_count, \
    sizeof(SymbolFunction), \
    symbol_function_init_data, \
    symbol_function_free_data \
)

typedef struct symbol_function_param_t {
    // inherit from symbolvariable
    char* name;
    DataType data_type;
    struct symbol_function_param_t* next;
    struct symbol_function_param_t* prev;
} SymbolFunctionParam;

typedef struct symbol_function_t {
    SymbolTableBaseItem base;

    bool declared;
    bool defined;
    DataType return_data_type;
    size_t arguments_count;
    SymbolFunctionParam* param;
    SymbolFunctionParam* param_tail;
} SymbolFunction;

/**
 * Get or create function symbol from symbol table.
 * @param table instance of table
 * @param key key to table
 * @return found or created symbol from table
 */
SymbolFunction* symbol_table_function_get_or_create(SymbolTable* table, const char* key);

/**
 * Try to get function symbol from table.
 * @param table instance
 * @param key key to found
 * @return function symbol or NULL
 */
SymbolFunction* symbol_table_function_get(SymbolTable* table, const char* key);

/**
 * Init data for function symbol - initialize to default member values.
 * @param item item to initialize
 */
void symbol_function_init_data(SymbolTableBaseItem* item);

/**
 * Frees custom data of symbol function.
 * @param item item to free
 */
void symbol_function_free_data(SymbolTableBaseItem* item);

/**
 * Add param to function symbol defined by name and data type.
 * @param function instance of function
 * @param name parameter name
 * @param data_type data type of parameter
 * @return created parameter
 */
SymbolFunctionParam* symbol_function_add_param(SymbolFunction* function, char* name, DataType data_type);

/**
 * Try to get parameter by given index. 0 is index of leftmost parameter.
 * @param function instance of function
 * @param index wanted index
 * @return param or NULL
 */
SymbolFunctionParam* symbol_function_get_param(SymbolFunction* function, size_t index);

/**
 * Try to find first declared function without definition. NULL if not found.
 * @param table instance
 * @return found function or NULL
 */
SymbolFunction* symbol_function_find_declared_function_without_definition(SymbolTable* table);

/**
 * Generates label to call function.
 * @param function instance
 * @return created label
 */
String* symbol_function_generate_function_label(SymbolFunction* function);

/**
 * For given param generates alias name for using as parameter name in target code - label includes index
 * and function name, not name itself (in moment of calling name could not be known).
 * @param function instance of param
 * @param param param to generate
 * @return generated alias name
 */
String* symbol_function_get_param_name_alias(SymbolFunction* function, SymbolFunctionParam* param);

#endif // _SYMTABLE_FUNCTION_H
