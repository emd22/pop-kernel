#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/drivers/boot_vga.h>

int normalize(double *val) {
    int exponent = 0;
    double value = *val;

    while (value >= 1.0) {
        value /= 10.0;
        ++exponent;
    }

    while (value < 0.1) {
        value *= 10.0;
        --exponent;
    }
    *val = value;
    return exponent;
}

void ftoa_fixed(char *buffer, double value) {  
    /* carry out a fixed conversion of a double value to a string, with a precision of 5 decimal digits. 
     * Values with absolute values less than 0.000001 are rounded to 0.0
     * Note: this blindly assumes that the buffer will be large enough to hold the largest possible result.
     * The largest value we expect is an IEEE 754 double precision real, with maximum magnitude of approximately
     * e+308. The C standard requires an implementation to allow a single conversion to produce up to 512 
     * characters, so that's what we really expect as the buffer size.     
     */

    int exponent = 0;
    int places = 0;
    static const int width = 4;

    if (value == 0.0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }         

    if (value < 0.0) {
        *buffer++ = '-';
        value = -value;
    }

    exponent = normalize(&value);

    while (exponent > 0) {
        int digit = value * 10;
        *buffer++ = digit + '0';
        value = value * 10 - digit;
        ++places;
        --exponent;
    }

    if (places == 0)
        *buffer++ = '0';

    *buffer++ = '.';

    while (exponent < 0 && places < width) {
        *buffer++ = '0';
        --exponent;
        ++places;
    }

    while (places < width) {
        int digit = value * 10.0;
        *buffer++ = digit + '0';
        value = value * 10.0 - digit;
        ++places;
    }
    *buffer = '\0';
}

int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char ch;
    int length = 0;

    char buffer[512];

    char tc;
    int ti;
    double td;
    char *ts;

    while ((ch = *fmt++)) {
        if (ch == '%') {
            switch (ch = *fmt++) {
                case '%':
                    bvga_put('%', BVGA_NONE);
                    length++;
                    break;
                case 'c':
                    tc = va_arg(args, int);
                    bvga_put(tc, BVGA_NONE);
                    length++;
                    break;
                case 's':
                    ts = va_arg(args, char *);
                    bvga_putstr(ts, BVGA_NONE);
                    length += strlen(ts);
                    break;
                case 'd':
                    ti = va_arg(args, int);
                    itoa(ti, buffer, 10);
                    bvga_putstr(buffer, BVGA_NONE);
                    length += strlen(buffer);
                    break;
                case 'x':
                    ti = va_arg(args, int);
                    itoa(ti, buffer, 16);
                    bvga_putstr(buffer, BVGA_NONE);
                    length += strlen(buffer);
                    break;
                case 'f':
                    td = va_arg(args, double);
                    ftoa_fixed(buffer, td);
                    bvga_putstr(buffer, BVGA_NONE);
                    length += strlen(buffer);
                    break;
            }
        }
        else if (ch == '\n') {
            bvga_nl();
        }
        else {
            bvga_put(ch, BVGA_NONE);
            length++;
        }
    }

    va_end(args);
    return length;
}

// int printf(const char *in, ...) {
//     va_list args;
//     va_start(args, in);

//     char c;

//     int index = 0;
//     /* I used the while over the pointer over a for loop
//        so it could all be done in one loop (no strlen).*/
//     while (*in != '\0') {
//         c = *in;
//         if (c == '%') {
//             if (c++ == 'c') {
//                 bvga_put(c, 0);
//             }
//             else if (c == 'd') {
//                 va_get()
//             }
//             else if (c == 's') {

//             }
//         }
//         in++;
//     }

//     va_end(args);
// }