#include <osutil.h>

#include <kernel/drivers/keyboard.h>

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
            buf[index+1] = 0;
            printf("%c", ch);  
        }
    }
}

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