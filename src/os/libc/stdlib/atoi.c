#include <stdlib.h>

int isspace(const char p) {
    return (p == ' ' || p == '\t') ? 1 : 0;
}

int atoi(const char *str) {
    register int result = 0;
    register unsigned int digit;
    int sign;

    /*
     * Skip any leading blanks.
     */

    while (isspace(*str))
        str += 1;

    if (*str == '-') {
        sign = 1;
        str += 1;
    } else {
        sign = 0;
        if (*str == '+') {
            str += 1;
        }
    }

    for (; ; str += 1) {
        digit = *str - '0';
        if (digit > 9) {
            break;
        }
        result = (10*result) + digit;
    }

    if (sign) {
        return -result;
    }
    return result;
}