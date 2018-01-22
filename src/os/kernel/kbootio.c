#include <kernel/kbootio.h>
#include <kernel/drivers/keyboard.h>
#include <stdlib.h>

static int index = 0;

void kinp(char *buf) {
    char ch = NULL;
    while (ch != '\n') {
        ch = getkey(NULL); //no flags, uses blocking input.
    }
}

void kinp_clr(void) {
    index = 0;
}