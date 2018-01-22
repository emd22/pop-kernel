#ifndef STDLIB_H
#define STDLIB_H

#if !defined(NULL)
    #define NULL ((void *)0)
#endif

char *itoa(int num, char *str, int base);

#endif