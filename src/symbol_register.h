#ifndef _SYMBOL_REGISTER_H
#define _SYMBOL_REGISTER_H

#include "symtable.h"
#include "symtable_variable.h"
#include "symtable_function.h"

/**
 * @brief Helper structure for stacking variable symbol tables.
 */
typedef struct symbol_table_symbol_variable_stack_item_t {
    SymbolTable* symbol_table;
    size_t scope_identifier;
    struct symbol_table_symbol_variable_stack_item_t* parent;
} SymbolTableSymbolVariableStackItem;

/**
 * @brief Register of used functions/variables.
 */
typedef struct symbol_register_t {
    SymbolTable* functions;
    SymbolTableSymbolVariableStackItem* variables;
    size_t variables_table_counter;
} SymbolRegister;

/**
 * Initialize symbol register with both tables.
 * @return initialized register
 */
SymbolRegister* symbol_register_init();

/**
 * Free symbol register with both tables.
 * @param register_ Symbol register
 */
void symbol_register_free(SymbolRegister** register_);

/**
 * Add new variables table and actual push backward.
 * @param register_ Symbol register
 */
void symbol_register_push_variables_table(SymbolRegister* register_);

/**
 * Remove actual variables table and second table marks as actual.
 * @param register_ Symbol register
 */
void symbol_register_pop_variables_table(SymbolRegister* register_);

/**
 * Try to find variable by name in actual variables table.
 * @param register_ Symbol register
 * @param key variable name to find
 * @return Found variable symbol or NULL
 */
SymbolVariable* symbol_register_find_variable(SymbolRegister* register_, const char* key);

/**
 * Try to find variable by name in all variable tables.
 * @param register_ Symbol register
 * @param key variable name to find
 * @return Found variable symbol or NULL
 */
SymbolVariable* symbol_register_find_variable_recursive(SymbolRegister* register_, const char* key);

/**
 * Create variable in current scope (controlled by stack depth), with assign to correct scope.
 * @param register_ Symbol register
 * @param key name of variable
 * @return created SymbolVariable
 */
SymbolVariable* symbol_register_new_variable(SymbolRegister* register_, const char* key);

#endif //_SYMBOL_REGISTER_H
