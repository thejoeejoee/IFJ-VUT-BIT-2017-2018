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

int hex_to_int(char char_) {

    if((int) char_ >= '0' && (int) char_ <= '9')
        return (int) char_ - '0';
    if((int) char_ >= 'a' && (int) char_ <= 'f')
        return (int) char_ - 'a' + 10;

    LOG_WARNING("Invalid char %c to convert.", char_);
    return 0;

}