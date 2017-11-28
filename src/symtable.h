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
typedef void(* symtable_free_data_callback_f)(SymbolTableBaseItem*);

typedef void(* symtable_init_data_callback_f)(SymbolTableBaseItem*);

typedef void(* symtable_copy_data_callback_f)(SymbolTableBaseItem*, SymbolTableBaseItem*);

typedef void(* symtable_foreach_callback_f)(const char* key, void* data, void* static_data);

typedef struct symbol_table_t {
    size_t item_size;
    size_t bucket_count;
    size_t item_count;
    symtable_free_data_callback_f free_data_callback;
    symtable_init_data_callback_f init_data_callback;
    symtable_copy_data_callback_f copy_data_callback;
    SymbolTableBaseItem* items[];
} SymbolTable;

/**
 * Construct new hash table with given size.
 * @param size count of buckets
 * @param item_size size of one item to correct memory allocating for inherited symtables
 * @param init_data_callback call back to init data for item
 * @param free_data_callback call back to free data for item
 * @return Ptr to allocated hash table, NULL in case of error.
 */
SymbolTable* symbol_table_init(
        size_t size,
        size_t item_size,
        symtable_init_data_callback_f init_data_callback,
        symtable_free_data_callback_f free_data_callback
);

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
 * Create new item with copied key, non linked to table.
 * @param key key to copy
 * @param item_size bytes to allocated
 */
SymbolTableBaseItem* symbol_table_new_item(const char* key, size_t item_size);

/**
 * Call given function on all items in hash table.
 */
void symbol_table_foreach(SymbolTable* table, symtable_foreach_callback_f, void* static_data);

/**
 * Try to remove item from table by given key.
 * @return true, if item was found and removed, else false
 */
bool symbol_table_remove(SymbolTable* table, const char* key);

/**
 * Dealloc all items with key from given hash table.
 */
void symbol_table_clear_buckets(SymbolTable* table);

// TODO doc
SymbolTable* symbol_table_copy(SymbolTable* other);

#endif //_SYMTABLE_H
