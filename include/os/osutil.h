#ifndef OSUTIL_H
#define OSUTIL_H

#define OS_NULL 0

#define OS_VERSION_MAJOR 0
#define OS_VERSION_MINOR 0
#define OS_VERSION_PATCH 1

#include <stdio.h>
#include <string.h>

#define assert(cond, msg) assert__(cond ? 1 : 0, msg)

void panic(const char *msg, void (*sd_cb)(void)) {
    printf("***KERNEL PANIC***:\n%s\nHALTING...\n", msg);
    if (sd_cb != NULL)
        sd_cb(); //shutdown callback(free objs, etc.)
    for (;;);
}

void assert__(int res, const char *msg) {
    if (res) return;
    char full_msg[256];
    bzero(full_msg, 256);
    strcpy(full_msg, "Assertion failed with message: ");
    strcat(full_msg, msg);
    panic(full_msg, NULL);
}

#endif