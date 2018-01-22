#ifndef STDIO_H
#define STDIO_H

#include <kernel/boot_vga.h>

#if !defined(NULL)
    #define NULL ((void *)NULL)
#endif

#define EOF (-1)

int printf(const char *fmt, ...);

#endif