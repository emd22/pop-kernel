#ifndef OSUTIL_H
#define OSUTIL_H

#define OS_NULL 0

#define OS_VERSION_MAJOR 0
#define OS_VERSION_MINOR 0
#define OS_VERSION_PATCH 2

#ifdef __i386__
#define OS_x86
#else
#define OS_x64
#endif

#include <stdio.h>
#include <string.h>

#define assert(cond, msg, cb) assert__((cond) ? 1 : 0, msg, cb)

void input(char *buf);
void assert__(int res, const char *msg, void (*gr_cb)(void));
void panic(const char *msg, void (*sd_cb)(void));

#endif