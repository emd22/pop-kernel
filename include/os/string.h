#ifndef STRING_H
#define STRING_H

#if !defined(NULL)
    #define NULL ((void *)0)
#endif

#include <stddef.h>
#define bzero(ptr, amt) memset(ptr, 0, amt);

size_t strlen(const char *str);
void *memset(void *ptr, int val, size_t num);
void *memcpy(void *dst_, const void *src_, size_t length);
char *strcpy(char *s1, const char *s2);
char *strcat(char *dest, const char *src);

#endif