#include "symtable_variable.h"
#include "symtable_function.h"

SymbolVariable* symbol_table_variable_get_or_create(SymbolTable* table, const char* key) {
    // internal!!!
    SymbolVariable* variable = (SymbolVariable*) symbol_table_get_or_create(table, key);
    return variable;
}

SymbolVariable* symbol_table_variable_get(SymbolTable* table, const char* key) {
    return (SymbolVariable*) symbol_table_get(table, key);
}

void symbol_variable_free_data(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);
    SymbolVariable* variable = (SymbolVariable*) item;
    memory_free(variable->base.key);
    variable->base.key = NULL;
    if(variable->alias_name != NULL) {
        memory_free(variable->alias_name);
        variable->alias_name = NULL;
    }
}

void symbol_variable_init_data(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);
    SymbolVariable* variable = (SymbolVariable*) item;
    variable->frame = VARIABLE_FRAME_NONE;
    variable->data_type = DATA_TYPE_NONE;
    variable->scope_depth = 0;
    variable->alias_name = NULL;
}

SymbolVariable* symbol_variable_copy(SymbolVariable* variable) {
    // NULL_POINTER_CHECK(variable, NULL);
    // deep copy of variable, without link to next
    SymbolVariable* new = symbol_variable_init(variable->base.key);
    symbol_variable_init_data((SymbolTableBaseItem*) new);

    new->data_type = variable->data_type;
    new->frame = variable->frame;
    new->scope_depth = variable->scope_depth;
    new->base.next = NULL;
    if(variable->alias_name != NULL) {
        new->alias_name = c_string_copy(variable->alias_name);
    }

    return new;
}

SymbolVariable* symbol_variable_init_from_function_param(SymbolFunction* function, SymbolFunctionParam* param) {
    NULL_POINTER_CHECK(function, NULL);
    NULL_POINTER_CHECK(param, NULL);

    SymbolVariable* variable = symbol_variable_init(param->name);
    symbol_variable_init_data((SymbolTableBaseItem*) variable);

    String* param_name = symbol_function_get_param_name_alias(function, param);

    variable->alias_name = c_string_copy(string_content(param_name));
    variable->frame = VARIABLE_FRAME_TEMP;
    variable->scope_depth++;

    string_free(&param_name);
    return variable;
}

void symbol_variable_single_free(SymbolVariable** variable) {
    NULL_POINTER_CHECK(variable,);
    NULL_POINTER_CHECK(*variable,);
    if((*variable)->base.next != NULL) {
        LOG_WARNING("Symbol variable from table, not standalone!");
    }
    symbol_variable_free_data((SymbolTableBaseItem*) *variable);
    memory_free(*variable);
    *variable = NULL;
}
