#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "symtable_typed.h"

// GENERIC DEFINITION FOR HASH TABLE
/**
 * Callback, which frees data pointer from hash table item.
 */
typedef void(* free_data_callback_f)(void*);

typedef struct symbol_table_list_item_t {
    char* key;
    void* data;
    struct symbol_table_list_item_t* next;
} SymbolTableListItem;

typedef struct symbol_table_t {
    size_t bucket_count;
    size_t item_count;
    free_data_callback_f free_data_callback;
    SymbolTableListItem* items[];
} SymbolTable;

/**
 * Construct new hash table with given size.
 * @return Ptr to allocated hash table, NULL in case of error.
 */
SymbolTable* symbol_table_init(size_t size, free_data_callback_f free_data_callback);

/**
 * Dealloc table from memory.
 * @param HTable
 */
void symbol_table_free(SymbolTable* table);

/**
 * Getter for actual hash table size.
 * @return Count of stored items.
 */
size_t symbol_table_size(SymbolTable* table);

/**
 * Getter for count of buckets in hash table.
 * @return Count of buckets in table.
 */
size_t symbol_table_bucket_count(SymbolTable* table);

/**
 * Construct new hash table and copy items from given table into it.
 * @return Ptr to allocated hash table, NULL in case of error.
 */
SymbolTable* symbol_table_move(size_t new_size, SymbolTable* table);

/**
 * Try to find item in hash table by given key. If is item not found,
 * new item is created and inserted to hash table.
 * @return Ptr to created/found item in hash table.
 */
SymbolTableListItem* symbol_table_get_or_create(SymbolTable* table, const char* key);

/**
 * Try to find item in hast table by given key.
 * @return Ptr to found item or NULL.
 */
SymbolTableListItem* symbol_table_get(SymbolTable* table, const char* key);

/**
 * Call given function on all items in hash table.
 */
void symbol_table_foreach(SymbolTable* table, void(* callback)(const char*, void*));

/**
 * Try to remove item from table by given key.
 * @return true, if item was found and removed, else false
 */
bool symbol_table_remove(SymbolTable* table, const char* key);

/**
 * Dealloc all items with key from given hash table.
 */
void symbol_table_clear_buckets(SymbolTable* table);

// SPECIFIC DEFINITIONS FOR SYMBOL TABLE IMPLEMENTATIONS

typedef struct symbol_variable_t {
    // TODO: define all needed members
    bool declared;
    bool defined;
    short data_type;
} SymbolVariable;

SYMBOL_TABLE_TYPED_HEADERS(SymbolVariable, symbol_variable)

typedef struct symbol_function_t {
    // TODO: define all needed members
    bool declared;
    bool defined;
    short return_data_type;
    SymbolTableSymbolVariable* variables;
} SymbolFunction;

SYMBOL_TABLE_TYPED_HEADERS(SymbolFunction, symbol_function)

#endif //_SYMTABLE_H
