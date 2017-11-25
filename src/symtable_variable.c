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
    if(variable->scope_alias != NULL) {
        memory_free(variable->scope_alias);
        variable->scope_alias = NULL;
    }
    if(variable->_cached_identifier != NULL) {
        memory_free(variable->_cached_identifier);
        variable->_cached_identifier = NULL;
    }
}

void symbol_variable_init_data(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);
    SymbolVariable* variable = (SymbolVariable*) item;
    variable->frame = VARIABLE_FRAME_NONE;
    variable->data_type = DATA_TYPE_NONE;
    variable->scope_depth = 0;
    variable->alias_name = NULL;
    variable->scope_alias = NULL;
    variable->_cached_identifier = NULL;
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
    if(variable->scope_alias != NULL) {
        new->scope_alias = c_string_copy(variable->scope_alias);
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
    variable->scope_alias = c_string_copy(function->base.key);
    variable->frame = VARIABLE_FRAME_TEMP;

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

SymbolVariable* symbol_variable_init_flag_for_static_variable(
        SymbolVariable* static_variable,
        SymbolFunction* function
) {
    NULL_POINTER_CHECK(static_variable, NULL);
    NULL_POINTER_CHECK(function, NULL);

    String* declaration_flag_variable_name = string_init();
    string_append_s(declaration_flag_variable_name, "IS_DECLARED__");
    string_append_s(declaration_flag_variable_name, function->base.key);
    string_append_s(declaration_flag_variable_name, "_");
    string_append_s(declaration_flag_variable_name, static_variable->base.key);

    SymbolVariable* declaration_flag_variable = symbol_variable_init(string_content(declaration_flag_variable_name));
    symbol_variable_init_data((SymbolTableBaseItem*) declaration_flag_variable);
    string_free(&declaration_flag_variable_name);

    declaration_flag_variable->frame = VARIABLE_FRAME_GLOBAL;
    return declaration_flag_variable;
}

bool symbol_variable_cmp(SymbolVariable* first, SymbolVariable* second)
{
    if(first->frame != second->frame)
        return false;

    if((first->alias_name != second->alias_name && first->alias_name == NULL) ||
            (first->alias_name == NULL && strcmp(first->base.key, second->base.key) != 0) ||
            (first->alias_name != NULL && strcmp(first->alias_name, second->alias_name) != 0))
        return false;

    if((first->scope_alias != second->scope_alias && first->scope_alias == NULL) ||
            (first->scope_alias == NULL && first->scope_depth != second->scope_depth) ||
            (first->scope_alias != NULL && strcmp(first->scope_alias, second->scope_alias) != 0))
        return false;

    return true;
}
