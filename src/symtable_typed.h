#ifndef _SYMTABLE_TYPED_H
#define _SYMTABLE_TYPED_H

#define HASH_TABLE_TYPED_ITEM(Type, type) \
typedef struct hash_table_list_item_## type ##_t {\
    char* key;\
    Type* data;\
    struct hash_table_list_item_## type ##_t* next;\
} HashTableListItem##Type

#define HASH_TABLE_TYPED_TABLE(Type, type) \
typedef struct hash_table_## type ##_t {\
    size_t bucket_count;\
    size_t item_count;\
    free_data_callback_f free_data_callback;\
    HashTableListItem##Type* items[];\
} HashTable##Type

#define HASH_TABLE_TYPED_METHODS(Type, type) \
HashTable##Type* hash_table_##type##_free(HashTable##Type* table) {\
    hash_table_free((HashTable*) table);\
}\
HashTable##Type* hash_table_##type##_init(size_t size, free_data_callback_f free_data_callback) {\
    return (HashTable##Type*) hash_table_init(size, free_data_callback);\
}\
HashTableListItem##Type* hash_table_##type##_get_or_create(HashTable##Type* table, const char* key) {\
    return (HashTableListItem##Type*) hash_table_get_or_create((HashTable*) table, key);\
}\
HashTableListItem##Type* hash_table_##type##_get(HashTable##Type* table, const char* key) {\
    return (HashTableListItem##Type*) hash_table_get((HashTable*) table, key);\
}

#define HASH_TABLE_TYPED(Type, type)\
HASH_TABLE_TYPED_ITEM(Type, type);\
HASH_TABLE_TYPED_TABLE(Type, type);\
HASH_TABLE_TYPED_METHODS(Type, type)

#endif //_SYMTABLE_TYPED_H
