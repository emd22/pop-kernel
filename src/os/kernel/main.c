#if defined(__linux__)
#error "Error: Not compiled using cross-compiler."
#endif

#if !defined(__i386__)
#error "Error: Kernel needs an ix86 system."
#endif

#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/boot_vga.h>
#include <kernel/debug.h>
#include <kernel/mem2d.h>
#include <kernel/args.h>
#include <osutil.h>
#include <stdio.h>
#include <stdint.h>

#define MAX_ARGS 32
#define MAX_ARG_LEN 32

void invalid_command(char **args) {
    bvga_set_colour(BVGA_ERR);
    printf("Invalid command '%s'\n", args[0]);
    bvga_set_colour(BVGA_DEF);
}

void kmain(void) {
    bvga_init();
    keyboard_init();

    char buf[64];

    char **args = alloc_2d(MAX_ARG_LEN, MAX_ARGS);
    int arg_len = 0;

    printf("Welcome to PopKernel v%d.%d.%d!\n\n", OS_VERSION_MAJOR, OS_VERSION_MINOR, OS_VERSION_PATCH);

    for (;;) {
        bvga_putstr("Pop", bvga_get_colour(BVGA_CYAN, BVGA_BLACK));
        bvga_putstr("Kernel", bvga_get_colour(BVGA_WHITE, BVGA_BLACK));
        bvga_putstr("# ", bvga_get_colour(BVGA_LIGHT_GREEN, BVGA_BLACK));
        
        input(buf);
        arg_len = split_to_args(buf, args);

        if (arg_len == 1 && !strcmp(args[0], "exit")) {
            bvga_clear();
            printf("it is now safe to shut down your computer.\n");
            break;
        }
        else if (arg_len == 1 && !strcmp(args[0], "clear")) {
            bvga_clear();
            buf[0] = 0;
        }
        else {
            invalid_command(args);
        }
    } 

    free_2d(MAX_ARGS, args);
}