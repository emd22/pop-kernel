#ifndef STDLIB_H
#define STDLIB_H

#if !defined(NULL)
    #define NULL ((void *)0)
#endif

#include <stddef.h>

void *malloc(size_t size);
void *calloc(size_t num, size_t size);
void *realloc(void *blk, size_t size);
void free(void *blk);

char *itoa(int num, char *str, int base);
int atoi(const char *str);

#endif