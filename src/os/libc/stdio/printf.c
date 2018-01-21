#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include <kernel/boot_vga.h>

int printf(const char *in, ...) {
    va_list args;
    va_start(args, in);

    char c;

    int index = 0;
    /* I used the while over the pointer over a for loop
       so it could all be done in one loop (no strlen).*/
    while (*in != '\0') {
        c = *in;
        if (c == '%') {
            if (c++ == 'c') {
                bvga_put(c, 0);
            }
            else if (c == 'd') {

            }
            else if (c == 's') {

            }
        }
        in++;
    }

    va_end(args);
}