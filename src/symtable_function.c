#include "symtable_function.h"

SymbolFunction* symbol_table_function_get_or_create(SymbolTable* table, const char* key) {
    SymbolFunction* symbol_function_return = (SymbolFunction*) symbol_table_get_or_create(table, key);
    return symbol_function_return;
}

SymbolFunction* symbol_table_function_get(SymbolTable* table, const char* key) {
    return (SymbolFunction*) symbol_table_get(table, key);
}

void symbol_function_free_data(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);
    SymbolFunctionParam* param = ((SymbolFunction*) item)->param;
    SymbolFunctionParam* next;
    while(param != NULL) {
        next = param->next;
        memory_free(param->name);
        memory_free(param);
        param = next;
    }
}

void symbol_function_init_data(SymbolTableBaseItem* item) {
    SymbolFunction* function = (SymbolFunction*) item;

    function->return_data_type = DATA_TYPE_NONE;
    function->param = NULL;
    function->declared = false;
    function->defined = false;
}

SymbolFunctionParam* symbol_function_add_param(SymbolFunction* function, char* name, DataType data_type) {
    NULL_POINTER_CHECK(function, NULL);
    SymbolFunctionParam* param = memory_alloc(sizeof(SymbolFunctionParam));

    param->data_type = data_type;
    param->name = c_string_copy(name);
    param->next = NULL;

    SymbolFunctionParam* actual = function->param;
    if(actual == NULL) {
        function->param = param;
    } else {
        while(actual->next != NULL) {
            actual = actual->next;
        }
        actual->next = param;
    }

    return param;
}

SymbolFunctionParam* symbol_function_get_param(SymbolFunction* function, size_t index) {
    NULL_POINTER_CHECK(function, NULL);

    SymbolFunctionParam* param = function->param;
    size_t actual = 0;

    do {
        if(actual == index)
            return param;
        actual++;
    } while(NULL != (param = param->next));
    return NULL;
}

void symbol_function_find_first_undefined_function_foreach(const char* key, void* item, void* static_data) {
    NULL_POINTER_CHECK(key,);
    NULL_POINTER_CHECK(item,);
    NULL_POINTER_CHECK(static_data,);

    SymbolFunction* function = (SymbolFunction*) item;
    char** function_name = ((char**) static_data);
    if(*function_name != NULL)
        return;

    if(function->declared && !function->defined) {
        *function_name = (char*) key;
    }
}

SymbolFunction* symbol_function_find_declared_function_without_definition(SymbolTable* table) {
    NULL_POINTER_CHECK(table, NULL);

    char* function_name = NULL;

    symbol_table_foreach(table, symbol_function_find_first_undefined_function_foreach, &function_name);
    if(function_name == NULL)
        return NULL;
    return symbol_table_function_get(table, function_name);
}
