#include "dynamic_string.h"
#include "memory.h"
#include "debug.h"
#include <string.h>

String string_new() {
	String tmp;
	tmp._size = 1;
	tmp._content = (char*) memory_alloc(sizeof(char)*STRING_INITIAL_CAPACITY);
	//TODO: null pointer check
	if (tmp._content == NULL) {
		tmp._capacity = 0;
	}
	else {
		tmp._content[0] = 0; //c-string terminator
		tmp._capacity = STRING_INITIAL_CAPACITY;
	}
	return tmp;
}

String string_new_cap(size_t initial_capacity)
{
	ASSERT(initial_capacity > 1); //smaller capacity than 2 makes no sense
	String tmp;
	tmp._size = 1;
	tmp._content = (char*)memory_alloc(sizeof(char)*initial_capacity);
	//TODO: null pointer check
	if (tmp._content == NULL) {
		tmp._capacity = 0;
	}
	else {
		tmp._content[0] = 0; //c-string terminator
		tmp._capacity = initial_capacity;
	}
	return tmp;
}

void string_delete(String *string) {
	NULL_POINTER_CHECK(string, )
	NULL_POINTER_CHECK(string->_content, )

	memory_free(string->_content);
}

// helper function for allocating more space if needed
inline void string_update_capacity(String* string, size_t needed_size) {
	NULL_POINTER_CHECK(string,)
	if (needed_size > string->_capacity) {
		size_t new_capacity = 2 * string->_capacity;
		new_capacity = new_capacity >= 2 ? new_capacity : 2;
		while (needed_size > new_capacity) {
			new_capacity <<= 2; //multiply new capacity by 2
			//TODO: check overflow
		}
		//TODO: realloc, memory pool?
		char* tmp = (char*) memory_alloc(sizeof(char) * new_capacity);
		//TODO: check malloc
		strcpy(tmp, string->_content);
		memory_free(string->_content);
		string->_capacity = new_capacity;
		string->_content = tmp;
	}
}

void string_append_c(String* string, const char c) {
	size_t new_size = string->_size + 1;
	string_update_capacity(string, new_size);
	ASSERT(string->_capacity >= new_size);
	ASSERT(string->_size >= 1);
	string->_content[string->_size - 1] = c;
	string->_content[new_size - 1] = 0;
	string->_size = new_size;
}

void string_append_s(String* string, const char* s) {
	size_t new_size = string->_size + strlen(s);
	string_update_capacity(string, new_size);
	ASSERT(string->_capacity >= new_size);
	ASSERT(string->_size >= 1);
	strcpy(&string->_content[string->_size - 1], s);
	string->_content[new_size - 1] = 0;
	string->_size = new_size;
}

void string_append(String* string, const String* b) {
	size_t new_size = string->_size + b->_size - 1;
	string_update_capacity(string, new_size);
	ASSERT(string->_capacity >= new_size);
	ASSERT(string->_size >= 1);
	strcpy(&string->_content[string->_size - 1], b->_content);
	string->_content[new_size - 1] = 0;
	string->_size = new_size;
}
