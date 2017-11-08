#include "common.h"
#include "memory.h"

char* c_string_copy(const char* string) {
    NULL_POINTER_CHECK(string, NULL);

    char* copied = memory_alloc(sizeof(char) * (strlen(string) + 1));

    if(NULL == strcpy(copied, string)) {
        LOG_WARNING("C-string copy failed, source: '%s'.", string);
        return NULL;
    }

    return copied;
}

int radix_to_int(char char_value) {

    if((int) char_value >= '0' && (int) char_value <= '9')
        return (int) char_value - '0';
    if((int) char_value >= 'a' && (int) char_value <= 'e')
        return (int) char_value - 'a' + 10;

    return 0;

}