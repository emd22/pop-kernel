#include <kernel/drivers/ata_pio.h>
#include <kernel/sysasm.h>
#include <stdbool.h>
#include <stdlib.h>
#include <osutil.h>

#define BAR0 0x1F0

#define PRIMARY_BUS  BAR0+6

#define COMMAND_PORT BAR0+7
#define STATUS_PORT  COMMAND_PORT
#define DATA_PORT    BAR0

#define MASTER_DRIVE 0xA0

#define SECTOR_COUNT BAR0+2
#define LBA_LOW      BAR0+3
#define LBA_MID      BAR0+4
#define LBA_HIGH     BAR0+5

#define READ_SECTORS 0x20);


#define MASTER 0xE0
#define SLAVE  0x0F

#define CACHE_FLUSH 0xE7

struct {
    uint16_t disk_info[256];
    unsigned sec_count;
} info;

void wait_ready(void) {
    while ((inb(COMMAND_PORT) & (0x80 | 0x40)) != 0x40);
}

void ata_pio_install(void) {
    //select drive MASTER on primary bus
    outb(PRIMARY_BUS, MASTER_DRIVE);

    //set sector count, lba low, lba mid, and lba high io ports to NULL
    outb(SECTOR_COUNT, OS_NULL);
    outb(LBA_LOW, OS_NULL);
    outb(LBA_MID, OS_NULL);
    outb(LBA_HIGH, OS_NULL);

    outb(COMMAND_PORT, 0xEC);

    uint8_t drive_exists = inb(COMMAND_PORT);

    if (drive_exists == 0){
        printf("Drive does not exist.\n");
        return;
    }

    wait_ready();

    uint8_t drive_pos = 0;

    drive_pos = inb(LBA_MID);
    drive_pos += inb(LBA_HIGH);

    //TODO: remove assert, make it use ramdisk or something
    assert(drive_pos == 0, "Disk is not PATA compatible.", NULL);

    wait_ready();

    int i;

    for (i = 0; i < 256; i++)
        info.disk_info[i] = inw(DATA_PORT);

    info.sec_count = info.disk_info[60];
    info.sec_count = info.sec_count << 16;
    info.sec_count += info.disk_info[61];
}

void cache_flush(void) {
    outb(COMMAND_PORT, CACHE_FLUSH);
    wait_ready();
}

void wait_400ns(void) {
    inb(COMMAND_PORT);
    inb(COMMAND_PORT);
    inb(COMMAND_PORT);
    inb(COMMAND_PORT);
}

void clean_up(void) {
    outb(0x1F6, 0);
    outb(SECTOR_COUNT, 0);
    outb(LBA_LOW, 0);
    outb(LBA_MID, 0);
    outb(LBA_HIGH, 0);
    outb(COMMAND_PORT, 0);
    outb(BAR0, 0);    
}

void ata_pio_rw(unsigned lba, uint16_t bp, void *buf, unsigned rw, unsigned sl_dl) {
    //sl_dl -> sector_length for reading, data length(bytes) for writing.
    while ((inb(STATUS_PORT) & 0xC0) != 0x40);
    // while ((inb(COMMAND_PORT) & (0x80 | 0x40)) != 0x40);
    outb(SECTOR_COUNT, sl_dl);

    //Send the low 8 bits of the LBA to port 0x1F3:
    outb(LBA_LOW, (uint8_t)lba);
    //Send the next 8 bits of the LBA to port 0x1F4:
    outb(LBA_MID, (uint8_t)(lba >> 8));
    //Send the next 8 bits of the LBA to port 0x1F5:
    outb(LBA_HIGH, (uint8_t)(lba >> 16));

    outb(COMMAND_PORT, rw);

    // while (!(inb(STATUS_PORT) & 0x80));

    if (rw == READ) {
        insb(DATA_PORT, buf, sl_dl*512);
    } 
    else if (rw == WRITE) {
        outsb(DATA_PORT, buf, sl_dl);
    }

    cache_flush();
}

void ata_pio_read(uint16_t lba, void *buf, int sc) {
    ata_pio_rw(lba, buf, READ, sc);
}

void ata_pio_write(uint16_t lba, const uint8_t *buf, size_t dl) {
    ata_pio_rw(lba, (void *)buf, WRITE, dl);
}
