#include <osutil.h>

void panic(const char *msg, void (*sd_cb)(void)) {
    printf("***KERNEL PANIC***:\n%s\nHALTING...\n", msg);
    if (sd_cb != NULL)
        sd_cb(); //shutdown callback(free objs, etc.)
    for (;;);
}

void assert__(int res, const char *msg, void (*gr_cb)(void)) {
    if (res) return;
    char full_msg[256];
    strcpy(full_msg, "Assertion failed with message: ");
    strcat(full_msg, msg);
    strcat(full_msg, "\0");
    panic(full_msg, gr_cb);
}