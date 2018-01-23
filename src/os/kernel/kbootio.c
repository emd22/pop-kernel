#include <kernel/kbootio.h>
#include <kernel/drivers/keyboard.h>
#include <stdlib.h>
#include <stdio.h>
#include <osutil.h>

void kinp(char *buf, int flags) {
    char ch = 0;

    int index = 0;

    while (1) {
        ch = getkey(OS_NULL);

        if (ch == '\n') {
            printf("\n");
            break;
        }
        
        else if (ch == KBDK_BACKSPACE) {
            buf[index--] = 0;
            bvga_mov_cur(-2, 0);
            printf("%c", ' ');
            continue;
        }

        if (!(flags & KINP_NOECHO)) {
            printf("%c", ch);
        }
        
        buf[index++] = ch;
    }
}