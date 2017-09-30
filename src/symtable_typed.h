#ifndef _SYMTABLE_TYPED_H
#define _SYMTABLE_TYPED_H

#include "memory.h"

#define SYMBOL_TABLE_TYPED_ITEM(Type, type) \
typedef struct symbol_table_list_item_## type ##_t {\
    char* key;\
    Type* data;\
    struct symbol_table_list_item_## type ##_t* next;\
} SymbolTableListItem## Type

#define SYMBOL_TABLE_TYPED_TABLE(Type, type) \
typedef void(* free_data_callback_## type ##_f)(Type*);\
typedef struct symbol_table_## type ##_t {\
    size_t bucket_count;\
    size_t item_count;\
    free_data_callback_## type ##_f free_data_callback;\
    SymbolTableListItem## Type* items[];\
} SymbolTable## Type

#define SYMBOL_TABLE_TYPED_METHOD_HEADERS(Type, type) \
SymbolTable## Type* symbol_table_## type ##_init(size_t size);\
void symbol_table_## type ##_free(SymbolTable## Type* table);\
void symbol_table_## type ##_free_callback(void*);\
SymbolTableListItem## Type* symbol_table_## type ##_get_or_create(SymbolTable## Type* table, const char* key);\
SymbolTableListItem## Type* symbol_table_## type ##_get(SymbolTable## Type* table, const char* key)

#define SYMBOL_TABLE_TYPED_IMPLEMENTATION(Type, type)\
    SYMBOL_TABLE_TYPED_INIT(Type, type)\
    SYMBOL_TABLE_TYPED_FREE_CALLBACK(Type, type)\
    SYMBOL_TABLE_TYPED_FREE(Type, type)\
    SYMBOL_TABLE_TYPED_GET_OR_CREATE(Type, type)\
    SYMBOL_TABLE_TYPED_GET(Type, type)

#define SYMBOL_TABLE_TYPED_INIT(Type, type) \
SymbolTable## Type* symbol_table_## type ##_init(size_t size) {\
    return (SymbolTable## Type*) symbol_table_init(size, symbol_table_## type ##_free_callback);\
}
#define SYMBOL_TABLE_TYPED_FREE_CALLBACK(Type, type) \
void symbol_table_## type ##_free_callback(void* data) {\
    /** Auto free of allocated data blocks. */\
    memory_free(data);\
}
#define SYMBOL_TABLE_TYPED_FREE(Type, type) \
void symbol_table_## type ##_free(SymbolTable## Type* table) {\
    symbol_table_free((SymbolTable*) table);\
}
#define SYMBOL_TABLE_TYPED_GET_OR_CREATE(Type, type) \
SymbolTableListItem## Type* symbol_table_## type ##_get_or_create(SymbolTable## Type* table, const char* key) {\
    SymbolTableListItem## Type* item = (SymbolTableListItem## Type*) symbol_table_get_or_create((SymbolTable*) table, key);\
    if (item->data == NULL)\
        /** Auto alloc for used data block. */\
        item->data = (Type*) memory_alloc(sizeof(Type));\
    return item;\
}
#define SYMBOL_TABLE_TYPED_GET(Type, type)\
SymbolTableListItem## Type* symbol_table_## type ##_get(SymbolTable## Type* table, const char* key) {\
    return (SymbolTableListItem## Type*) symbol_table_get((SymbolTable*) table, key);\
}

#define SYMBOL_TABLE_TYPED_HEADERS(Type, type)\
    SYMBOL_TABLE_TYPED_ITEM(Type, type);\
    SYMBOL_TABLE_TYPED_TABLE(Type, type);\
    SYMBOL_TABLE_TYPED_METHOD_HEADERS(Type, type)

#endif //_SYMTABLE_TYPED_H
