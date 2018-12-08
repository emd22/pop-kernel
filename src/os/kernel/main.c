#if defined(__linux__)
#error "Error: Not compiled using cross-compiler."
#endif

#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/vga_text.h>
#include <kernel/drivers/pci.h>
#include <kernel/drivers/ide.h>
#include <kernel/drivers/mbr.h>
#include <kernel/drivers/irq.h>
#include <kernel/drivers/idt.h>
#include <kernel/drivers/isrs.h>
#include <kernel/drivers/gdt.h>
#include <kernel/memory/mm.h>
#include <kernel/memory/paging.h>
#include <kernel/time.h>
#include <kernel/args.h>
#include <kernel/hd.h>
#include <osutil.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

uint32_t kernel_end;
uint32_t kernel_base;

uint8_t ata_initd = 0;

static char **args;

void invalid_command(char **args) {
    bvga_set_colour(BVGA_ERR);
    printf("Invalid command '%s'\n", args[0]);
    bvga_set_colour(BVGA_DEF);
}

int check_command(char **args, const char *command) {
    return !strcmp(args[0], command);
}

char **alloc_2d(int x, int y) {
    char **arr;
    arr = malloc(y*sizeof(char *));

    int i;
    for (i = 0; i < y; i++) {
        arr[i] = malloc(x);
    }
    return arr;
}

void free_2d(int y, char **arr) {
    int i;
    for (i = 0; i < y; i++) {
        free(arr[i]);
    }
    free(arr);
}

void setup(void) {
    mm_init((uint32_t)(&kernel_end));
    paging_init();
    bvga_init();
    
    gdt_install();
    idt_install();
    isrs_install();
    irq_install();
    keyboard_init();

    pci_init();
    pci_recursive_check();

    int pci_dev_amt;
    pci_dev_t **pci_devices;

    pci_devices = pci_get_devices(&pci_dev_amt);

    hd_init(pci_devices, pci_dev_amt);

    drive_t *drives;
    int drive_index;
    drives = hd_get_drives(&drive_index);
    ide_init(drives, &drive_index);

    ide_set_bus(0, 0);
}

void command_line(void) {
    char buf[64];

    input(buf);
    char **args = alloc_2d(MAX_ARG_LEN, MAX_ARGS);
    int arg_len = split_to_args(buf, args);

    if (check_command(args, "clear")) {
        bvga_clear();
        buf[0] = 0;
    }
    else if (check_command(args, "write")) {
        if (arg_len != 3) {
            printf("Usage: write <drive index> <data>\n");
            return;
        }
        drive_t *drives;
        int drive_index;
        drives = hd_get_drives(&drive_index);

        printf("%d\n", drive_index);

        int set_index = atoi(args[1]);
        if (set_index > drive_index) {
            printf("Drive index too great!\n");
            return;
        }
        if (drives[set_index].controller_type != IDE_CONTROLLER) {
            printf("Not IDE Hard Disk Controller!\n");
            return;
        }

        uint8_t buf[512];
        memset(buf, 0, 512);
        strcpy((char *)buf, args[2]);
        drives[set_index].write_block(1, 1, buf);

        memset(buf, 0, 512);
        drives[set_index].read_block(1, 1, buf);
        printf("buf:(%s)\n", buf);
    }
    else {
        invalid_command(args);
    }
    clear_args(args, arg_len);
}

char **main_loop(void) {
    for (;;) {
        // cmos_td_t cmos_td = cmos_rtc_gettime();
        // printf("%d :: %s\n", cmos_td.hour, time_str(&cmos_td, TIME_ALL));

        bvga_putstr("Pop", bvga_get_colour(BVGA_CYAN, BVGA_BLACK));
        bvga_putstr("Kernel", bvga_get_colour(BVGA_WHITE, BVGA_BLACK));
        bvga_set_colour(bvga_get_colour(BVGA_CYAN, BVGA_BLACK));
        printf(" %c ", 282);
        bvga_set_colour(BVGA_DEF);

        command_line();
    }
}

void kmain(void) {
    setup();

    mm_inf_t mm_inf = get_mm_inf();

    printf("Available RAM: %d bytes\n", mm_inf.heap_end-mm_inf.heap_begin);

    bvga_set_colour(bvga_get_colour(BVGA_CYAN, BVGA_BLACK));
    printf("Welcome to PopKernel v%d.%d.%d!\n\n", OS_VERSION_MAJOR, OS_VERSION_MINOR, OS_VERSION_PATCH);
    bvga_set_colour(BVGA_DEF);

    pci_dev_t **pci_devices;
    pci_dev_t *cur_dev;
    int pci_amt_devs;

    pci_devices = pci_get_devices(&pci_amt_devs);

    int i;
    for (i = 0; i < pci_amt_devs; i++) {
        cur_dev = pci_devices[i];
        printf("dev: 0x%x:0x%x -- 0x%x:0x%x\n", cur_dev->vendor_id, cur_dev->device_id, cur_dev->class_code, cur_dev->subclass_code);
    }

    main_loop();

    free_2d(MAX_ARGS, args);
}
