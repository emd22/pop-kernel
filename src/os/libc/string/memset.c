#include <string.h>

void *memset(void *ptr, int val, size_t num) {
    if (num) {
        char *p = ptr;

        do {
            *p++ = val;
        } while (--num);
    }
    return ptr;
}