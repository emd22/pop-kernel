#include <kernel/debug.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/boot_vga.h>
#include <stdio.h>

void input(char *buf) {
    char ch = 0;
    int index = 0;

    while (true) {
        ch = getkey(KBD_NOBLOCK);

        if (ch == 0) continue;

        if (ch == '\n') {
            buf[index++] = 0;
            printf("\n");
            break;
        }
        if (ch == KBDK_BACKSPACE) {
            if (index > 0) {
                bvga_mov_cur(-1, 0);
                bvga_put(' ', BVGA_DEF, BVGA_NOMOVE);
                buf[index--] = 0;
            }
        }
        else {
            buf[index++] = ch;
            printf("%c", ch);  
        }
    }
}

void debug_err(const char *msg) {
    //TODO: timestamp? + other info
    bvga_putstr(msg, BVGA_ERR);
}