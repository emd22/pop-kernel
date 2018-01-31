#if defined(__linux__)
#error "Error: Not compiled using cross-compiler."
#endif

#if !defined(__i386__)
#error "Error: Kernel needs an ix86 system."
#endif

#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/boot_vga.h>
#include <kernel/drivers/sata.h>
#include <kernel/drivers/mbr.h>
#include <kernel/debug.h>
#include <kernel/mem2d.h>
#include <kernel/args.h>
#include <osutil.h>
#include <stdio.h>
#include <stdint.h>

void invalid_command(char **args) {
    bvga_set_colour(BVGA_ERR);
    printf("Invalid command '%s'\n", args[0]);
    bvga_set_colour(BVGA_DEF);
}

int check_command(char **args, const char *command) {
    return !strcmp(args[0], command);
}

uint64_t bar5;

void kmain(void) {
    bvga_init();
    keyboard_init();
    // ata_pio_install();
    mbr_init();
    sata_init();

    HBA_PORT hba_port;

    if (!(abar->ghc & BIT31))
        abar->ghc |= BIT31;

    // port_rebase(&hba_port, 0);
    probe_port((HBA_MEM *)(0xFFFFFFFF00000000+(uintptr_t)bar5));
    // retrieve_partitions();

    // uint8_t buf1[512];

    // ata_pio_read(10000, buf1);
    // int i;
    // for (i = 0; i < 8; i++) {
    //     printf("d:%c\n", buf1[i]);
    // }

    // char buf[64];

    // char **args = alloc_2d(MAX_ARG_LEN, MAX_ARGS);
    // int arg_len = 0;

    // printf("Welcome to PopKernel v%d.%d.%d!\n\n", OS_VERSION_MAJOR, OS_VERSION_MINOR, OS_VERSION_PATCH);

    // for (;;) {
    //     bvga_putstr("Pop", bvga_get_colour(BVGA_CYAN, BVGA_BLACK));
    //     bvga_putstr("Kernel", bvga_get_colour(BVGA_WHITE, BVGA_BLACK));
    //     bvga_putstr("# ", bvga_get_colour(BVGA_LIGHT_GREEN, BVGA_BLACK));
        
    //     input(buf);
    //     arg_len = split_to_args(buf, args);

    //     if (check_command(args, "exit")) {
    //         bvga_clear();
    //         printf("it is now safe to shut down your computer.\n");
    //         break;
    //     }
    //     else if (check_command(args, "clear")) {
    //         bvga_clear();
    //         buf[0] = 0;
    //     }
    //     else if (check_command(args, "mbrload")) {
    //         retrieve_partitions();
    //     }
    //     else if (check_command(args, "mbrprg")) {
    //         mbr_purge();
    //     }
    //     else if (check_command(args, "mbrfmt")) {
    //         format_mbr();
    //         mbr_write_part(get_part_entry(0), 1000, 5000, FAT32_SYSID); //4kb
    //     }
    //     else {
    //         invalid_command(args);
    //     }
    //     clear_args(args, arg_len);
    // } 

    // free_2d(MAX_ARGS, args);
}