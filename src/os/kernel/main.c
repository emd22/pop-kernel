#if defined(__linux__)
#error "Error: Not compiled using cross-compiler."
#endif

#if !defined(__i386__)
#error "Error: Kernel needs an ix86 system."
#endif

#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/boot_vga.h>
#include <kernel/drivers/pci.h>
#include <kernel/drivers/ahci.h>
#include <kernel/drivers/mbr.h>
#include <kernel/drivers/irq.h>
#include <kernel/drivers/idt.h>
#include <kernel/memory/mem2d.h>
#include <kernel/memory/mm.h>
#include <kernel/memory/paging.h>
#include <kernel/time.h>
#include <kernel/debug.h>
#include <kernel/args.h>
#include <kernel/err.h>
#include <kernel/drivers/gdt.h>
#include <osutil.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define KERN_VMBASE (0xffff808000000000ull)

bool ahci_exists;
uint32_t kernel_end;
uint32_t kernel_base;

void invalid_command(char **args) {
    bvga_set_colour(BVGA_ERR);
    printf("Invalid command '%s'\n", args[0]);
    bvga_set_colour(BVGA_DEF);
}

void ascii(char **args, int arg_len) {
    if (arg_len == 2) {
        int c = atoi(args[1]);
        printf("Ascii: %c\n", c);
    }
    else if (arg_len == 3) {
        int until = atoi(args[2]);
        int start = atoi(args[1]);
        if (until >= start) {
            bvga_clear();
            int i;

            for (i = start; i < until; i++) {
                printf("(%d)", i);
                bvga_set_colour(bvga_get_colour(BVGA_LIGHT_RED, BVGA_BLACK));
                printf("%c ", i);
                bvga_set_colour(BVGA_DEF);

            }
        }
        printf("\nEND\n");
    }
    else {
        printf("Not enough arguments! -> [ascii] [65] [optional: until]\n");                
    }
}

int check_command(char **args, const char *command) {
    return !strcmp(args[0], command);
}

void kmain(void) {
    mm_init(&kernel_end);
    paging_init();
    
    gdt_install();
    idt_install();
    irq_install();

    bvga_init();
    keyboard_init();
    ahci_init();
    mbr_init();

    mm_inf_t mm_inf = get_mm_inf();

    printf("Available RAM: %d bytes\n", mm_inf.heap_end-mm_inf.heap_begin);

    uint32_t bar5 = pci_brute_force();
    while(1);
    paging_map(KERN_VMBASE+bar5, bar5);

    HBA_MEM *abar = /* (HBA_MEM *)(KERN_VMBASE + bar5) */NULL;

    abar = (HBA_MEM  *)(unsigned)(inl((unsigned)(bar5+0x24)));
    printf("ABAR = %d, BAR5 = %d\n", &abar, bar5);

    // void *abar = malloc(4096);
    ahci_probe_port(abar);

    uint8_t *buf_ = (uint8_t *)malloc(sizeof(uint8_t)*512);
    
    bzero(buf_, 512);
    // const char *error;
    // if ((error = read(0, 0, 1, buf_)) != NULL) {
    //     printf("AHCI ERROR: %s\n", error);
    // }
    if (read(&abar->ports[0], 0, 0, 1, buf_)) {
        int i;
        for (i = 0; i < 512; i++) {
            if (buf_[i] != 0) {
                printf("[%d]", buf_[i]);
            }
        }
        printf("\nREADING COMPLETED\n");
    }

    

    free(buf_);

    char buf[64];

    char **args = alloc_2d(MAX_ARG_LEN, MAX_ARGS);
    int arg_len = 0;

    bvga_set_colour(bvga_get_colour(BVGA_CYAN, BVGA_BLACK));
    printf("Welcome to PopKernel v%d.%d.%d!\n\n", OS_VERSION_MAJOR, OS_VERSION_MINOR, OS_VERSION_PATCH);
    bvga_set_colour(BVGA_DEF);

    for (;;) {
        bvga_putstr("Pop", bvga_get_colour(BVGA_CYAN, BVGA_BLACK));
        bvga_putstr("Kernel", bvga_get_colour(BVGA_WHITE, BVGA_BLACK));
        bvga_set_colour(bvga_get_colour(BVGA_CYAN, BVGA_BLACK));
        printf(" %c ", 282);
        // printf("# ");
        bvga_set_colour(BVGA_DEF);
        
        input(buf);
        arg_len = split_to_args(buf, args);

        if (check_command(args, "exit")) {
            bvga_clear();
            printf("it is now safe to shut down your computer %c\n", 258);
            break;
        }
        else if (check_command(args, "clear")) {
            bvga_clear();
            buf[0] = 0;
        }
        else if (check_command(args, "ascii")) {
            ascii(args, arg_len);
        }
        // else if (check_command(args, "mbrload")) {
        //     retrieve_partitions();
        // }
        // else if (check_command(args, "mbrprg")) {
        //     mbr_purge();
        // }
        // else if (check_command(args, "mbrfmt")) {
        //     if (format_mbr())
        //         mbr_write_part(get_part_entry(0), 1000, 5000, FAT32_SYSID); //4kb
        // }
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
