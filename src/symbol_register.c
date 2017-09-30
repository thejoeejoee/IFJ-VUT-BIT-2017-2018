#include "symbol_register.h"

SymbolRegister* symbol_register_init() {
    SymbolRegister* register_ = (SymbolRegister*) memory_alloc(sizeof(SymbolRegister));

    // TODO: sizes?
    register_->functions = symbol_table_function_init(8);
    register_->variables = symbol_table_variable_init(16);

    return register_;
}

void symbol_register_free(SymbolRegister** register_) {
    NULL_POINTER_CHECK(register_,);
    NULL_POINTER_CHECK(*register_,);

    symbol_table_function_free((*register_)->functions);
    symbol_table_variable_free((*register_)->variables);
    memory_free(*register_);
    *register_ = NULL;
}

void symbol_register_push_variables_table(SymbolRegister* register_) {
    NULL_POINTER_CHECK(register_,);

    SymbolTableSymbolVariable* table = symbol_table_variable_init(16);
    // table->parent = register_->variables;
    register_->variables = table;
}

void symbol_register_pop_variables_table(SymbolRegister* register_) {
    NULL_POINTER_CHECK(register_,);
    NULL_POINTER_CHECK(register_->variables,);

    SymbolTableSymbolVariable* to_free = register_->variables;
    // if(to_free->parent != NULL) {
    //     register_->variables = to_free->parent;
    // }
    memory_free(to_free);
}

SymbolVariable* symbol_register_find_variable(SymbolRegister* register_, const char* key) {
    NULL_POINTER_CHECK(register_, NULL);
    NULL_POINTER_CHECK(key, NULL);

    SymbolTableListItemSymbolVariable* item = symbol_table_variable_get_or_create(register_->variables, key);
    if(item == NULL)
        return NULL;
    return item->data;
}

SymbolVariable* symbol_register_find_variable_recursive(SymbolRegister* register_, const char* key) {
    NULL_POINTER_CHECK(register_, NULL);
    NULL_POINTER_CHECK(key, NULL);

    SymbolTableListItemSymbolVariable* item = NULL;
    SymbolTableSymbolVariable* variables = register_->variables;

    while(variables != NULL) {
        item = symbol_table_variable_get_or_create(variables, key);
        if(item != NULL)
            return item->data;

        // variables = variables->parent;
    }
    return NULL;
}
