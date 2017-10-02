#include "symbol_register.h"

SymbolRegister* symbol_register_init() {
    SymbolRegister* register_ = (SymbolRegister*) memory_alloc(sizeof(SymbolRegister));

    // TODO: sizes?
    register_->functions = symbol_table_function_init(8);
    register_->variables = memory_alloc(sizeof(SymbolTable));
    register_->variables->symbol_table = symbol_table_variable_init(16);
    register_->variables->parent = NULL;

    return register_;
}

void symbol_register_free(SymbolRegister** register_) {
    NULL_POINTER_CHECK(register_,);
    NULL_POINTER_CHECK(*register_,);

    symbol_table_free((*register_)->functions);

    SymbolTableSymbolVariableStackItem* stack_item = (*register_)->variables;
    SymbolTableSymbolVariableStackItem* parent;

    while(stack_item != NULL) {
        symbol_table_free(stack_item->symbol_table);
        parent = stack_item->parent;
        memory_free(stack_item);
        stack_item = parent;
    }

    memory_free(*register_);
    *register_ = NULL;
}

void symbol_register_push_variables_table(SymbolRegister* register_) {
    NULL_POINTER_CHECK(register_,);

    SymbolTableSymbolVariableStackItem* stack = memory_alloc(sizeof(SymbolTableSymbolVariableStackItem));
    stack->symbol_table = symbol_table_variable_init(16);
    stack->parent = register_->variables;
    register_->variables = stack;
}

void symbol_register_pop_variables_table(SymbolRegister* register_) {
    NULL_POINTER_CHECK(register_,);
    NULL_POINTER_CHECK(register_->variables,);

    SymbolTableSymbolVariableStackItem* stack_item_to_free = register_->variables;
    register_->variables = stack_item_to_free->parent;
    symbol_table_free(stack_item_to_free->symbol_table);
    memory_free(stack_item_to_free);

    if(register_->variables == NULL) {
        // poped last stack item
        register_->variables = memory_alloc(sizeof(SymbolTableSymbolVariableStackItem));
        register_->variables->symbol_table = symbol_table_variable_init(16);
        register_->variables->parent = NULL;
    }
}

SymbolVariable* symbol_register_find_variable(SymbolRegister* register_, const char* key) {
    NULL_POINTER_CHECK(register_, NULL);
    NULL_POINTER_CHECK(register_->variables, NULL);
    NULL_POINTER_CHECK(register_->variables->symbol_table, NULL);
    NULL_POINTER_CHECK(key, NULL);

    SymbolVariable* item = symbol_table_variable_get(
            register_->variables->symbol_table,
            key
    );
    if(item == NULL)
        return NULL;
    return item;
}

SymbolVariable* symbol_register_find_variable_recursive(SymbolRegister* register_, const char* key) {
    NULL_POINTER_CHECK(register_, NULL);
    NULL_POINTER_CHECK(register_->variables, NULL);
    NULL_POINTER_CHECK(register_->variables->symbol_table, NULL);
    NULL_POINTER_CHECK(key, NULL);

    SymbolVariable* item = NULL;
    SymbolTableSymbolVariableStackItem* variables = register_->variables;

    while(variables != NULL) {
        item = symbol_table_variable_get(variables->symbol_table, key);
        if(item != NULL)
            return item;

        variables = variables->parent;
    }
    return NULL;
}
