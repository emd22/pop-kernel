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
#include <kernel/err.h>
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

void kmain(void) {
    bvga_init();
    keyboard_init();
    // ata_pio_install();
    mbr_init();
    ahci_init();

    ahci_detect();

    HBA_PORT *port = get_port();

    uint8_t buf[512];
    bzero(buf, 512);

    // uint8_t out[] = {'T', 'E', 'S', 'T', '\0'};

    // memcpy(buf, out, 5);

    // ahci_write(buf, 1000, 1);

    // bzero(buf, 512);

    /* 
    chk_err runs this function(made as macro so it could return any type + have any param types) 
    and checks the return type. it will kernel panic if error is found.

    chk_err(<function>, params...);
    */
    chk_err(read, port, 0, 0, 1, buf);

    // sata_write();

    // HBA_PORT hba_port;

    // port_rebase(&hba_port, 0);
    // probe_port((HBA_MEM *)(0xFFFFFFFF00000000+(uintptr_t)bar5));
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