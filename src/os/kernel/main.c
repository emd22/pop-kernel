#if defined(__linux__)
#error "Error: Not compiled using cross-compiler."
#endif

#if !defined(__i386__)
#error "Error: Kernel needs an ix86 system."
#endif

#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/boot_vga.h>
#include <kernel/drivers/pci.h>
#include <kernel/drivers/sata.h>
#include <kernel/drivers/mbr.h>
#include <kernel/drivers/irq.h>
#include <kernel/drivers/idt.h>
#include <kernel/memory/mem2d.h>
#include <kernel/memory/mm.h>
#include <kernel/memory/memory.h>
#include <kernel/time.h>
#include <kernel/debug.h>
#include <kernel/args.h>
#include <kernel/err.h>
#include <kernel/drivers/gdt.h>
#include <osutil.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


bool ahci_exists;
uint32_t kernel_end;
uint32_t kernel_base;

void invalid_command(char **args) {
    bvga_set_colour(BVGA_ERR);
    printf("Invalid command '%s'\n", args[0]);
    bvga_set_colour(BVGA_DEF);
}

int check_command(char **args, const char *command) {
    return !strcmp(args[0], command);
}

void kmain(void) {
    printf("Kbase: 0x%x, Kend: 0x%x\n", &kernel_base, &kernel_end);
    mm_init(&kernel_end);
    paging_init();
    
    gdt_install();
    idt_install();
    irq_install();

    bvga_init();
    keyboard_init();
    pci_init();
    ahci_init();
    mbr_init();

    void *abar = malloc(4096);

    probe_port(abar);

    ahci_exists = ahci_detect();

    assert(ahci_exists, "AHCI not found.", NULL);

    uint8_t *buf_ = (uint8_t *)malloc(sizeof(uint8_t)*512);
    
    bzero(buf_, 512);
    const char *error;
    if ((error = read(0, 0, 1, buf_)) != NULL) {
        printf("AHCI ERROR: %s\n", error);
    }

    int i;
    for (i = 0; i < 512; i++) {
        if (buf_[i] != 0) {
            printf("[%d]", buf_[i]);
        }
    }
    printf("\nREADING COMPLETED\n");

    free(buf_);

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

        if (check_command(args, "exit")) {
            bvga_clear();
            printf("it is now safe to shut down your computer.\n");
            break;
        }
        else if (check_command(args, "clear")) {
            bvga_clear();
            buf[0] = 0;
        }
        else if (check_command(args, "mbrload")) {
            retrieve_partitions();
        }
        else if (check_command(args, "mbrprg")) {
            mbr_purge();
        }
        else if (check_command(args, "mbrfmt")) {
            if (format_mbr())
                mbr_write_part(get_part_entry(0), 1000, 5000, FAT32_SYSID); //4kb
        }
        else if (check_command(args, "td")) {
            cmos_td_t c_td = cmos_rtc_gettime();
            printf("Date: %s %s, '%d\n", time_get_month(c_td.month), time_day_full(c_td.day), c_td.year);
            printf("Time: %s\n", time_str(&c_td, TIME_HOUR | TIME_MINUTE));
        }
        else {
            invalid_command(args);
        }
        clear_args(args, arg_len);
    } 

    free_2d(MAX_ARGS, args);
}
