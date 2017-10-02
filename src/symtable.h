#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "memory.h"

typedef struct symbol_table_base_list_item_t {
    char* key;
    struct symbol_table_base_list_item_t* next;
} SymbolTableBaseItem;

/**
 * Callback, which frees data pointer from hash table item.
 */
typedef void(* free_data_callback_f)(SymbolTableBaseItem*);

typedef void(* init_data_callback_f)(SymbolTableBaseItem*);

typedef struct symbol_table_t {
    size_t item_size;
    size_t bucket_count;
    size_t item_count;
    free_data_callback_f free_data_callback;
    init_data_callback_f init_data_callback;
    SymbolTableBaseItem* items[];
} SymbolTable;

/**
 * Construct new hash table with given size.
 * @return Ptr to allocated hash table, NULL in case of error.
 */
SymbolTable* symbol_table_init(size_t size, size_t item_size,
                               init_data_callback_f init_data_callback,
                               free_data_callback_f free_data_callback);

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
SymbolTableBaseItem* symbol_table_get_or_create(SymbolTable* table, const char* key);

/**
 * Try to find item in hast table by given key.
 * @return Ptr to found item or NULL.
 */
SymbolTableBaseItem* symbol_table_get(SymbolTable* table, const char* key);

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

#endif //_SYMTABLE_H
