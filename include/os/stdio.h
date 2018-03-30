#ifndef STDIO_H
#define STDIO_H

#include <kernel/drivers/boot_vga.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#define EOF (-1)

int printf(const char *fmt, ...);

#endif