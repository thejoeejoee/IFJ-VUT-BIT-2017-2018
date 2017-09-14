#ifndef _IAL_H
#define _IAL_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>


typedef struct hash_table_list_item_t {
    char* key;
    void* data;
    struct hash_table_list_item_t* next;
} HashTableListItem;


typedef struct hash_table_t {
    size_t arr_size;
    size_t n;
    HashTableListItem* items[];
} HashTable;

/**
 * Construct new hash table with given size.
 * @return Ptr to allocated hash table, NULL in case of error.
 */
HashTable* hash_table_init(size_t size);

/**
 * Construct new hash table and copy items from given table into it.
 * @return Ptr to allocated hash table, NULL in case of error.
 */
HashTable* hash_table_move(size_t new_size, HashTable* table);

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
 * Try to find item in hash table by given key. If is item not found,
 * new item is created and inserted to hash table.
 * @return Ptr to created/found item in hash table.
 */
HashTableListItem* hash_table_lookup_add(HashTable* table, const char* key);

/**
 * Try to find item in hast table by given key.
 * @return Ptr to found item or NULL.
 */
HashTableListItem* hash_table_find(HashTable* table, const char* key);

/**
 * Allocate memory for hash table item and associated key, which is copied from given.
 * @internal
 * @return Ptr to new item.
 */
HashTableListItem* hash_table_create_item(const char* key);

/**
 * Given item simply append to correct bucket of given table.
 */
void hash_table_append_item(HashTable* table, HashTableListItem* item);

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
void hash_table_clear(HashTable* table, void(* free_data)(void*));

/**
 * Dealloc table from memory.
 * @param HTable
 */
void hash_table_free(HashTable* table, void(* free_data)(void*));

/**
 * Header for hash function needed by this lib.
 * @param str string to hash
 * @return computed hash
 */
size_t hash(const char* str);

#endif //_IAL_H
