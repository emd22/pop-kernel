#include <kernel/drivers/ata_pio.h>
#include <stdbool.h>
#include <kernel/sysasm.h>
#include <osutil.h>


#define PRIMARY_BUS 0x1F6

#define IDENTIFY_PORT 0x1F7
#define DATA_PORT     0x1F0

#define MASTER_DRIVE 0xA0

#define SECTOR_COUNT 0x1F2
#define LBA_LOW      0x1F3
#define LBA_MID      0x1F4
#define LBA_HIGH     0x1F5

#define MASTER 0xE0
#define SLAVE  0x0F

struct {
    uint16_t disk_info[256];
    unsigned sec_count;
} info;

void ata_pio_install(void) {
    //select drive MASTER on primary bus
    outb(PRIMARY_BUS, MASTER_DRIVE);

    //set sector count, lba low, lba mid, and lba high io ports to NULL
    outb(SECTOR_COUNT, OS_NULL);
    outb(LBA_LOW, OS_NULL);
    outb(LBA_MID, OS_NULL);
    outb(LBA_HIGH, OS_NULL);

    outb(IDENTIFY_PORT, 0xEC);

    uint8_t drive_exists = inb(IDENTIFY_PORT);
    bool primary_detected = false;

    if (drive_exists != 0) {
        primary_detected = true;
    }
    else {
        printf("Drive does not exist.\n");
        return;
    }

    uint8_t drive_pos = 0;

    drive_pos = inb(LBA_MID);
    drive_pos += inb(LBA_HIGH);

    //TODO: remove assert, make it use ramdisk or something
    assert(drive_pos == 0, "Disk is not PATA compatible.");

    int i;

    for (i = 0; i < 256; i++)
        info.disk_info[i] = inw(DATA_PORT);

    info.sec_count = info.disk_info[60];
    info.sec_count = info.sec_count << 16;
    info.sec_count += info.disk_info[61];
}

void wait_ready(void) {
    while (inb(IDENTIFY_PORT) & (0x80 | 0x40) != 0x40);
}

void ata_pio_read(size_t lba, uint8_t *buf) {
    wait_ready();

    size_t bytes_read;
    uint16_t word;
    uint16_t slavebit = 0;

    //select master or slave
    outb(0x1F6, MASTER | (slavebit << 4) | ((lba >> 24) & SLAVE));
}