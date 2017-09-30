#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "symtable_typed.h"

/**
 * Callback, which frees data pointer from hash table item.
 */
typedef void(* free_data_callback_f)(void*);

typedef struct hash_table_list_item_t {
    char* key;
    void* data;
    struct hash_table_list_item_t* next;
} HashTableListItem;

typedef struct hash_table_t {
    size_t bucket_count;
    size_t item_count;
    free_data_callback_f free_data_callback;
    HashTableListItem* items[];
} HashTable;

typedef struct symbol_variable_t {
    // TODO: define all needed members
    bool declared;
    bool defined;
    short data_type;
} SymbolVariable;

/**
 * Construct new hash table with given size.
 * @return Ptr to allocated hash table, NULL in case of error.
 */
HashTable* hash_table_init(size_t size, free_data_callback_f free_data_callback);

/**
 * Dealloc table from memory.
 * @param HTable
 */
void hash_table_free(HashTable* table);

/**
 * Getter for actual hash table size.
 * @return Count of stored items.
 */
size_t hash_table_size(HashTable* table);

/**
 * Getter for count of buckets in hash table.
 * @return Count of buckets in table.
 */
size_t hash_table_bucket_count(HashTable* table);

/**
 * Construct new hash table and copy items from given table into it.
 * @return Ptr to allocated hash table, NULL in case of error.
 */
HashTable* hash_table_move(size_t new_size, HashTable* table);

/**
 * Try to find item in hash table by given key. If is item not found,
 * new item is created and inserted to hash table.
 * @return Ptr to created/found item in hash table.
 */
HashTableListItem* hash_table_get_or_create(HashTable* table, const char* key);

/**
 * Try to find item in hast table by given key.
 * @return Ptr to found item or NULL.
 */
HashTableListItem* hash_table_get(HashTable* table, const char* key);

/**
 * Call given function on all items in hash table.
 */
void hash_table_foreach(HashTable* table, void(* callback)(const char*, void*));

/**
 * Try to remove item from table by given key.
 * @return true, if item was found and removed, else false
 */
bool hash_table_remove(HashTable* table, const char* key);

/**
 * Dealloc all items with key from given hash table.
 */
void hash_table_clear_buckets(HashTable* table);

HASH_TABLE_TYPED_HEADERS(SymbolVariable, symbol_variable)

#endif //_SYMTABLE_H
