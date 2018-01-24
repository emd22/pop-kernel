#ifndef OSUTIL_H
#define OSUTIL_H

#define OS_NULL 0

#define OS_VERSION_MAJOR 0
#define OS_VERSION_MINOR 0
#define OS_VERSION_PATCH 1

#define ASSERT(cond) {if (!(cond)) panic("")}

void panic(const char *msg) {
    printf("***KERNEL PANIC***: %s\nHALTING...\n");
    for (;;);
}

#endif