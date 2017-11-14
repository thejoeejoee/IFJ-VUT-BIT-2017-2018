#include "symtable_variable.h"
#include "symtable_function.h"

static LList meta_data_shared_pointers;

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
    symbol_variable_meta_data_remove_reference(variable->meta_data);

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
}

void symbol_variable_init_data(SymbolTableBaseItem* item) {
    NULL_POINTER_CHECK(item,);
    SymbolVariable* variable = (SymbolVariable*) item;
    variable->frame = VARIABLE_FRAME_NONE;
    variable->data_type = DATA_TYPE_NONE;
    variable->scope_depth = 0;
    variable->alias_name = NULL;
    variable->scope_alias = NULL;
    variable->meta_data = symbol_variable_meta_data_init();
}

SymbolVariable* symbol_variable_copy(SymbolVariable* variable) {
    // NULL_POINTER_CHECK(variable, NULL);
    // deep copy of variable, without link to next
    SymbolVariable* new = symbol_variable_init(variable->base.key);
    symbol_variable_init_data((SymbolTableBaseItem*) new);

    symbol_variable_meta_data_free(&new->meta_data);

    new->meta_data = variable->meta_data;
    symbol_variable_meta_data_add_reference(variable->meta_data);
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
    symbol_variable_meta_data_add_reference(variable->meta_data);

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
    symbol_variable_meta_data_remove_reference((*variable)->meta_data);
    memory_free(*variable);
    *variable = NULL;
}

SymbolVariableMetaData* symbol_variable_meta_data_init()
{
    static bool shared_ptr_list_inited = false;
    if(!shared_ptr_list_inited) {
        llist_init_list(&meta_data_shared_pointers, sizeof(SharedPointer), &init_shared_pointer, &symbol_variable_meta_data_item_free, NULL);
        shared_ptr_list_inited = true;
    }

    SymbolVariableMetaData* meta_data = memory_alloc(sizeof(SymbolVariableMetaData));
    symbol_variable_meta_data_add_reference(meta_data);

    meta_data->occurrences_count = 0;
    meta_data->type = VARIABLE_META_TYPE_PURE;
    return meta_data;
}

void symbol_variable_meta_data_free(SymbolVariableMetaData** item)
{
    symbol_variable_meta_data_remove_reference(*item);
    *item = NULL;
}

void symbol_variable_meta_data_item_free(LListBaseItem* item)
{
    SharedPointer* sh = (SharedPointer*)item;
    if(sh->addr == NULL)
        return;

    memory_free(sh->addr);
}

void symbol_variable_meta_data_add_reference(SymbolVariableMetaData* item)
{
    add_reference(&meta_data_shared_pointers, item);
}

void symbol_variable_meta_data_remove_reference(SymbolVariableMetaData* item)
{
    remove_reference(&meta_data_shared_pointers, item);
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
