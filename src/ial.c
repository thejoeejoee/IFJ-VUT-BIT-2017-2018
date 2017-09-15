#include <assert.h>
#include "ial.h"

size_t hash(const char* str);

void hash_table_append_item(HashTable* table,
                            HashTableListItem* new_item) {
    NULL_POINTER_CHECK(table,);
    NULL_POINTER_CHECK(new_item,);

    size_t index = hash(new_item->key) % table->bucket_count;
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

void hash_table_clear_buckets(HashTable* table,
                              free_data_callback_f free_data) {
    NULL_POINTER_CHECK(table,);
    NULL_POINTER_CHECK(free_data,);

    HashTableListItem* item_to_free = NULL;
    HashTableListItem* tmp_item = NULL;

    for (size_t i = 0; i < table->bucket_count; ++i) {
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
    table->item_count = 0;
}

HashTableListItem* hash_table_new_item(const char* key) {
    NULL_POINTER_CHECK(key, NULL);
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

HashTableListItem* hash_table_get(HashTable* table, const char* key) {
    NULL_POINTER_CHECK(table, NULL);
    NULL_POINTER_CHECK(key, NULL);

    size_t index = hash(key) % table->bucket_count;
    HashTableListItem* item = table->items[index];

	while (item != NULL) {
		if (0 == strcmp(key, item->key))
			return item;
		item = item->next;
	}
	return item;
}

void hash_table_foreach(HashTable* table,
                        void(* callback)(const char*, void*)) {
    NULL_POINTER_CHECK(table,);
    NULL_POINTER_CHECK(callback,);

    HashTableListItem* item;
    for (size_t i = 0; i < table->bucket_count; ++i) {
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

    table->bucket_count = size;
    table->item_count = 0;
    for (size_t i = 0; i < size; ++i)
        table->items[i] = NULL;

    return table;
}

size_t hash_table_size(HashTable* table) {
    NULL_POINTER_CHECK(table, 0);
    return table->item_count;
}

size_t hash_table_bucket_count(HashTable* table) {
    NULL_POINTER_CHECK(table, 0);
    return table->bucket_count;
}

void hash_table_free(HashTable* table, free_data_callback_f free_data) {
    NULL_POINTER_CHECK(table,);
    hash_table_clear_buckets(table, free_data);

    free(table);
}

HashTableListItem* hash_table_get_or_create(HashTable* table, const char* key) {
    NULL_POINTER_CHECK(table, NULL);
    NULL_POINTER_CHECK(key, NULL);

    size_t index = hash(key) % table->bucket_count;
    HashTableListItem* item = table->items[index];

    if (item != NULL)
        while (42) {
            if (0 == strcmp(key, item->key))
                return item;

            if (item->next == NULL) break;

            item = item->next;
        };

    // key not found, we need to allocate new item
    HashTableListItem* new_item = hash_table_new_item(key);
    if (new_item == NULL) return NULL;

    if (item == NULL) {
        // not items not found at index
        table->items[index] = new_item;
    } else {
        // link to next
        item->next = new_item;
    }
    table->item_count++;

    return new_item;
}

HashTable* hash_table_move(size_t new_size, HashTable* source) {
    HashTable* destination = hash_table_init(new_size);
    if (destination == NULL) return NULL;

    destination->bucket_count = new_size;

    HashTableListItem* item = NULL;
    HashTableListItem* next = NULL;

    for (size_t source_bucket_i = 0; source_bucket_i < source->bucket_count; ++source_bucket_i) {
        item = source->items[source_bucket_i];
        source->items[source_bucket_i] = NULL;
        while (item != NULL) {
            next = item->next;
            hash_table_append_item(destination, item);
            item = next;
        };
    }

    destination->item_count = source->item_count;
    source->item_count = 0;

    return destination;
}

bool hash_table_delete(HashTable* table, const char* key,
                       free_data_callback_f free_data_callback) {
    NULL_POINTER_CHECK(table, NULL);
    NULL_POINTER_CHECK(key, NULL);
    NULL_POINTER_CHECK(free_data_callback, NULL);

    size_t index = hash(key) % table->bucket_count;
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
            free_data_callback(item->data);
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
