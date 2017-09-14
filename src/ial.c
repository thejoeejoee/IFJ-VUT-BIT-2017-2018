#include <assert.h>
#include "ial.h"

void hash_table_append_item(HashTable* table, HashTableListItem* new_item) {
    if (table == NULL || new_item == NULL)
        // TODO resolve states with NULL params
        return;

    size_t index = hash(new_item->key) % table->arr_size;
    HashTableListItem* target = table->items[index];

    new_item->next = NULL;
    if (target == NULL) {
        table->items[index] = new_item;

    } else {
        while (target->next != NULL) {
            target = target->next;
        };
        target->next = new_item;
    }
}

void hash_table_clear(HashTable* table, void(* free_data)(void*)) {
    if (table == NULL) return;
    assert(free_data != NULL);

    HashTableListItem* item_to_free = NULL,
            * tmp_item = NULL;

    for (size_t i = 0; i < table->arr_size; ++i) {
        item_to_free = table->items[i];
        if (item_to_free == NULL) continue;
        do {
            tmp_item = item_to_free;
            item_to_free = item_to_free->next;
            free(tmp_item->key);
            free_data(tmp_item->data);
            free(tmp_item);
        } while (item_to_free != NULL);
    }
    table->n = 0;
}

HashTableListItem* hash_table_create_item(const char* key) {
    HashTableListItem* new_item = NULL;
    char* copied_key = NULL;

    if (NULL == (new_item = (HashTableListItem*) malloc(sizeof(HashTableListItem))))
        return NULL;

    if (NULL == (copied_key = (char*) malloc(sizeof(char) * (strlen(key) + 1)))) {
        free(new_item);
        return NULL;
    }

    if (NULL == strcpy(copied_key, key)) {
        free(new_item);
        free(copied_key);
        return NULL;
    }

    new_item->key = copied_key;
    new_item->data = NULL;
    new_item->next = NULL;
    return new_item;
}

HashTableListItem* hash_table_find(HashTable* table, const char* key) {
    if (table == NULL || key == NULL) return NULL;

    size_t index = hash(key) % table->arr_size;
    HashTableListItem* item = table->items[index];

    if (item != NULL)
        while (1) {
            if (0 == strcmp(key, item->key))
                return item;

            if (item->next == NULL) break;

            item = item->next;
        };
    return NULL;
}

void hash_table_foreach(HashTable* table,
                        void(* callback)(const char*, void*)) {
    if (table == NULL || callback == NULL) return;

    HashTableListItem* item;
    for (size_t i = 0; i < table->arr_size; ++i) {
        item = table->items[i];
        while (item != NULL) {
            callback(item->key, &item->data);
            item = item->next;
        }
    }
}

HashTable* hash_table_init(size_t size) {
    HashTable* table;
    size_t need_memory = sizeof(HashTable) +
                         sizeof(HashTableListItem*) * size;

    if (NULL == (table = (HashTable*) malloc(need_memory))) return NULL;

    table->arr_size = size;
    table->n = 0;
    for (size_t i = 0; i < size; ++i)
        table->items[i] = NULL;

    return table;
}

long hash_table_size(HashTable* table) {
    if (table == NULL) return -1;
    return table->n;
}

long hash_table_bucket_count(HashTable* table) {
    if (table == NULL) return -1;
    return table->arr_size;
}

void hash_table_free(HashTable* table, void(* free_data)(void*)) {
    if (table == NULL) return;
    hash_table_clear(table, free_data);

    free(table);
}

HashTableListItem* hash_table_lookup_add(HashTable* table, const char* key) {
    if (table == NULL || key == NULL) return NULL;

    size_t index = hash(key) % table->arr_size;
    HashTableListItem* item = table->items[index];

    if (item != NULL)
        while (42) {
            if (0 == strcmp(key, item->key))
                return item;

            if (item->next == NULL) break;

            item = item->next;
        };

    // key not found, we need to allocate new item
    HashTableListItem* new_item = hash_table_create_item(key);
    if (new_item == NULL) return NULL;

    if (item == NULL) {
        // not items not found at index
        table->items[index] = new_item;
    } else {
        // link to next
        item->next = new_item;
    }
    table->n++;

    return new_item;
}

HashTable* hash_table_move(size_t new_size, HashTable* source) {
    HashTable* destination = hash_table_init(new_size);
    if (destination == NULL) return NULL;

    destination->arr_size = new_size;

    HashTableListItem* item = NULL;
    HashTableListItem* next = NULL;

    for (size_t source_bucket_i = 0; source_bucket_i < source->arr_size; ++source_bucket_i) {
        item = source->items[source_bucket_i];
        source->items[source_bucket_i] = NULL;
        while (item != NULL) {
            next = item->next;
            hash_table_append_item(destination, item);
            item = next;
        };
    }

    destination->n = source->n;
    source->n = 0;

    return destination;
}

bool hash_table_remove(HashTable* table, const char* key) {
    if (table == NULL || key == NULL) return NULL;

    size_t index = hash(key) % table->arr_size;
    HashTableListItem* item = table->items[index];
    HashTableListItem* prev = NULL;

    if (item == NULL) return false;

    while (42) {
        if (0 == strcmp(key, item->key)) {
            if (prev != NULL)
                prev->next = item->next;
            else
                table->items[index] = item;
            free(item->key);
            free(item);
            return true;
        }

        if (item->next == NULL) break;

        prev = item;
        item = item->next;
    };
    return false;
}

size_t hash(const char* str) {
    unsigned hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
