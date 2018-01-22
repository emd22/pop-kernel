#ifndef STRING_H
#define STRING_H

#if !defined(NULL)
    #define NULL ((void *)NULL)
#endif

#include <stddef.h>
#define bzero(ptr, amt) memset(ptr, 0, amt);

size_t strlen(const char *str);
void *memset(void *ptr, int val, size_t num);

#endif