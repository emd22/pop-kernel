#include <kernel/drivers/ata_pio.h>
#include <kernel/sysasm.h>
#include <stdbool.h>
#include <stdlib.h>
#include <osutil.h>

#define BAR0 0x1F0

#define PRIMARY_BUS  BAR0+6

#define COMMAND_PORT BAR0+7
#define DATA_PORT    BAR0

#define MASTER_DRIVE 0xA0

#define SECTOR_COUNT BAR0+2
#define LBA_LOW      BAR0+3
#define LBA_MID      BAR0+4
#define LBA_HIGH     BAR0+5

#define READ_SECTORS 0x20

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

int ata_pio_read(uint16_t lba, uint8_t *buf) {
    wait_ready();

    size_t bytes_read = 0;
    uint16_t word;
    uint16_t slavebit = 0;

    //select master or slave
    uint8_t ms_slv = MASTER | (slavebit << 4) | ((lba >> 24) & SLAVE);
    printf("ms_slv: %x\n", ms_slv);
    outb(0x1F6, ms_slv);
    outb(SECTOR_COUNT, 1); //ask for one sector

    //Send first 8 bits of the LBA to LBA_LOW
    outb(LBA_LOW, (uint8_t)lba);
    //Send the next 8 bits of the LBA to LBA_MID
    outb(LBA_MID, (uint8_t)(lba >> 8));
    //Send the next 8 bits of the LBA to LBA_HIGH
    outb(LBA_HIGH, (uint8_t)(lba >> 16));

    //send read sectors command
    outb(COMMAND_PORT, READ_SECTORS);

    int i;
    for (i = 0; i < 10000; i++); //wait for irq or poll

    wait_ready();

    while (bytes_read < 256) {
        word = inw(0x1F0);

        buf[bytes_read * 2] = word & 0xFF;
        buf[(bytes_read * 2) + 1] = word >> 8;

        bytes_read++;
    }
    // insm(BAR0, buf, 256);
    
    // clean_up();
    return bytes_read;
}

void ata_pio_write(uint16_t lba, const uint8_t *buf, size_t d_len) {
    int n_sectors = d_len/512;
    
    if (n_sectors == 0) 
        n_sectors = 1;

    outb(SECTOR_COUNT, (uint8_t)n_sectors); //set number of sectors
    //Send the low 8 bits of the LBA to port 0x1F3:
    outb(LBA_LOW, (uint8_t)lba);
    //Send the next 8 bits of the LBA to port 0x1F4:
    outb(LBA_MID, (uint8_t)(lba >> 8));
    //Send the next 8 bits of the LBA to port 0x1F5:
    outb(LBA_HIGH, (uint8_t)(lba >> 16));

    outb(COMMAND_PORT, 0x30); //initialize write command

    int i;
    for (i = 0; i < d_len; i++) {
        wait_400ns();
        outb(BAR0, buf[i]); //write to io port
        cache_flush();
        wait_ready();
    }

    // outsm(BAR0, buf, 256);

    wait_ready();
    wait_400ns();

    // ata_pio_read(lba, NULL);
    // clean_up();
}