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
    } while(++actual && NULL != (param = param->next));
    return NULL;
}
