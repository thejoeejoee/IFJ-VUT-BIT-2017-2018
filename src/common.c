#include "code_optimizer_expr.h"
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

int round_even(double x) {
    x -= remainder(x, 1.0);
    return (int) x;
}