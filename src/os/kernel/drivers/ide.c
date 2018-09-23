#include <kernel/drivers/ide.h>
#include <kernel/drivers/pci.h>
#include <kernel/arch_io.h>

#include <stdint.h>
#include <stdio.h>

#define ATA_SR_BSY  0x80
#define ATA_SR_ERR  0x01
#define ATA_SR_DRQ  0x08

#define ATA_REG_DATA      0x00
#define ATA_REG_ERROR     0x01
#define ATA_REG_SECCOUNT0 0x02
#define ATA_REG_LBA0      0x03
#define ATA_REG_LBA1      0x04
#define ATA_REG_LBA2      0x05
#define ATA_REG_HDDEVSEL  0x06
#define ATA_REG_COMMAND   0x07
#define ATA_REG_STATUS    0x07
#define ATA_REG_SECCOUNT1 0x08
#define ATA_REG_CONTROL   0x0C

#define ATA_CMD_CACHE_FLUSH 0xE7
#define ATA_CMD_READ_PIO  0x20
#define ATA_CMD_WRITE_PIO 0x30

#define ATA_DRV_DEFAULT 0xA0
#define ATA_DRV_LBA     0x40
#define ATA_DRV_SLAVE   0x10

#define ATA_MASTER 0
#define ATA_SLAVE  1

#define ATA_PRIMARY    0
#define ATA_SECONDARY  1

#define BAR0_PRIMARY_IO   0x1F0
#define BAR0_SECONDARY_IO 0x170

// static ide_drive_t ide_drives[4] = {0, 0, 0, 0};

static int bus = ATA_PRIMARY;
static int bus_position = ATA_MASTER;

// ide_drive_t *ide_drives_find(void) {
//     return ide_drives;
// }

int get_io(int port) {
    int io = (bus == ATA_PRIMARY ? 0x1F0 : 0x170);

    // #if 0
    if (port == ATA_REG_CONTROL) {
        //data field requires BAR1 instead of BAR0, increase by 204h to change the register.
        io += 0x204;
    }
    // #endif
    return io;
}

void io_out(uint16_t port, uint8_t val) {
    int io = get_io(port);
    outb(io+port, val);
}

uint8_t io_in(uint16_t port) {
    int io = get_io(port);
    return inb(io+port);
}

void io_outw(uint16_t port, uint16_t val) {
    int io = get_io(port);
    outw(io+port, val);
}

uint16_t io_inw(uint16_t port) {
    int io = get_io(port);
    return inw(io+port);
}

void wait_400ns(void) {
    io_in(ATA_REG_STATUS);
    io_in(ATA_REG_STATUS);
    io_in(ATA_REG_STATUS);
    io_in(ATA_REG_STATUS);
}

void ide_select_drive(unsigned lba) {
    //if bus pos(slave/master) is 1(slave) send 0xB0(IDE slave) command to select drive. else, send 0xA0(IDE master) command.
    io_out(ATA_REG_HDDEVSEL, ((bus_position == ATA_MASTER ? 0xA0 : 0xB0) | (uint8_t)((lba >> 24 & 0x0F))));
}

void select_sector(unsigned lba, uint16_t sector_count) {
    io_out(ATA_REG_SECCOUNT0, (uint8_t)sector_count);
    io_out(ATA_REG_SECCOUNT1, (uint8_t)(sector_count << 8));
    io_out(ATA_REG_LBA0, (uint8_t)lba);
    io_out(ATA_REG_LBA1, (uint8_t)(lba << 8));
    io_out(ATA_REG_LBA2, (uint8_t)(lba << 16));
}

uint8_t poll_command(void) {
    wait_400ns();
    
    uint8_t status = io_in(ATA_REG_STATUS);
    while ((status & ATA_SR_BSY) || (status & ATA_SR_ERR) || !(status & ATA_SR_DRQ)) {
        // printf("stat:%d\n", status & ATA_SR_DRQ);
        status = io_in(ATA_REG_STATUS);
        if ((status & ATA_SR_ERR) || (!(status & ATA_SR_DRQ)))
            return !(status & ATA_SR_DRQ) ? 3 : 2;
    }
    return 0;
}

void ide_write_block(unsigned lba, uint16_t sector_count, const uint8_t *data) {
    wait_400ns();

    ide_select_drive(lba);
    //set ATA_REG_ERROR(ATA_FEATURES) to zero in case of an error
    io_out(ATA_REG_ERROR, 0x00);
    select_sector(lba, sector_count);
    io_out(ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);
    
    uint8_t poll_stat;
    poll_stat = poll_command();
    if (poll_stat) {
        printf("stat write: %d\n", poll_stat);
        return; // status failed because of error.
    }
    
    int i;
    uint16_t cur;
    for (i = 0; i < sector_count*(512/2); i++) {
        // cur = (data[i+1]) | data[i];
        cur = (data[i*2+1] << 8) | data[i*2];        
        // printf("cur: %d|%d = %d\n", data[i*2+1], data[i*2], cur);
        io_outw(ATA_REG_DATA, cur);
        // io_out(ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
    }
    wait_400ns();
    // ide_select_drive(lba);
    io_out(ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
    poll_stat = poll_command();
    if (poll_stat != 0) {
        printf("stat wriet: %d\n", poll_stat);
        return; // status failed because of error.
    }
}

void ide_read_block(unsigned lba, uint16_t sector_count, uint8_t *data) {
    ide_select_drive(lba);
    //set ATA_REG_ERROR(ATA_FEATURES) to zero in case of an error
    io_out(ATA_REG_ERROR, 0x00);
    select_sector(lba, sector_count);
    io_out(ATA_REG_COMMAND, ATA_CMD_READ_PIO);
    
    uint8_t poll_stat;
    poll_stat = poll_command();
    if (poll_stat != 0) {
        printf("stat read: %d\n", poll_stat);
        return; // status failed because of error.
    }
    
    int i;
    uint16_t cur;
    for (i = 0; i < sector_count*(512/2); i++) {
        cur = io_inw(ATA_REG_DATA);
        // printf("cur=%c\n", cur);
        // data[i]  = (uint8_t)cur;
        // data[i+1] = (uint8_t)(cur << 8);
        *(uint16_t *)(data + i * 2) = cur;
        // printf("cur: %d|%d = %d\n", data[i+1], data[i], cur);
        
    }
    wait_400ns();
    io_out(ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
    poll_stat = poll_command();
    if (poll_stat != 0) {
        printf("stat reda: %d\n", poll_stat);
        return; // status failed because of error.
    } 
}