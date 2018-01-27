#if defined(__linux__)
#error "Error: Not compiled using cross-compiler."
#endif

#if !defined(__i386__)
#error "Error: Kernel needs an ix86 system."
#endif

#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/idt.h>
#include <kernel/drivers/boot_vga.h>
#include <kernel/drivers/ata_pio.h>
#include <osutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

void input(char *buf) {
    char ch = 0;
    int index = 0;

    while (true) {
        ch = getkey(KBD_NOBLOCK);

        if (ch == 0) continue;

        if (ch == '\n') {
            printf("\n");
            break;
        }
        if (ch == KBDK_BACKSPACE) {
            
            bvga_mov_cur(-1, 0);
            bvga_put_no_mv(' ', BVGA_DEF);
            buf[index--] = 0;
        }
        else {
            buf[index++] = ch;
            printf("%c", ch);  
        }
    }
}

void kmain(void) {
    bvga_init();
    keyboard_init();
    idt_install();
    ata_pio_install();

    char buf[64];

    input(buf);

    printf("GOT: [%s]\n", buf);
    // return;

    // ata_pio_write(1000, (uint8_t *)"Hello!", 7);

    // uint8_t *buf = (uint8_t *)malloc(512);

    // ata_pio_read(1000, buf);

    // // printf("dat:%s\n", buf);
    // int i = 0;
    // while (i < 7) {
    //     printf("[%c]\n", buf[i++]);
    // }

    // free(buf);
}