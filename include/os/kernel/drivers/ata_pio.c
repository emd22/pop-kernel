#include <kernel/drivers/ata_pio.h>
#include <stdbool.h>
#include <kernel/sysasm.h>
#include <osutil.h>

#define PRIMARY_BUS 0x1F6
#define IDENTIFY_PORT 0x1F7

#define MASTER_DRIVE 0xA0

struct {
    uint8_t *disk_info;
    unsigned sec_count;
} info;

void ata_pio_install(void) {
    //select drive MASTER on primary bus
    outb(PRIMARY_BUS, MASTER_DRIVE);

    //set sector count, lba low, lba mid, and lba high io ports to NULL
    outb(0x1F2, OS_NULL);
    outb(0x1F3, OS_NULL);
    outb(0x1F4, OS_NULL);
    outb(0x1F5, OS_NULL);

    outb(IDENTIFY_PORT, 0xEC);

    uint8_t drive_exists = inb(IDENTIFY_PORT);
    bool primary_detected = false;

    if (drive_exists != 0) {
        primary_detected = true;
    }
    else {
        pr
    }
}