#include <osutil.h>
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
#define ATA_REG_ALTSTATUS 0x0C

#define ATA_CMD_CACHE_FLUSH 0xE7
#define ATA_CMD_READ_PIO    0x20
#define ATA_CMD_WRITE_PIO   0x30
#define ATA_CMD_IDENTIFY    0xEC

#define ATA_DRV_DEFAULT 0xA0
#define ATA_DRV_LBA     0x40
#define ATA_DRV_SLAVE   0x10

#define ATA_MASTER 0
#define ATA_SLAVE  1

#define ATA_PRIMARY    0
#define ATA_SECONDARY  1

#define BAR0_PRIMARY_IO   0x1F0
#define BAR0_SECONDARY_IO 0x170

static int bus = ATA_PRIMARY;
static int bus_position = ATA_MASTER;
static unsigned current_lba = 0;

int get_io(int port) {
    int io = (bus == ATA_PRIMARY ? 0x1F0 : 0x170);

    if (port == ATA_REG_CONTROL) {
        //data field requires BAR1 instead of BAR0, increase by 204h to change the register.
        io += 0x204;
    }
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
    io_in(ATA_REG_ALTSTATUS);
    io_in(ATA_REG_ALTSTATUS);
    io_in(ATA_REG_ALTSTATUS);
    io_in(ATA_REG_ALTSTATUS);
}

void ide_select_drive(unsigned lba) {
    //if bus pos(slave/master) is 1(slave) send 0xB0(IDE slave) command to select drive. else, send 0xA0(IDE master) command.
    
    uint8_t val = (0xE0 | (bus_position << 4) | ((lba >> 24) & 0x0F));
    io_out(ATA_REG_HDDEVSEL, val);
}

void select_sector(unsigned lba, uint16_t sector_count) {
    io_out(ATA_REG_SECCOUNT0, (uint8_t)sector_count);
    io_out(ATA_REG_SECCOUNT1, (uint8_t)(sector_count >> 8));

    io_out(ATA_REG_LBA0, (uint8_t)lba);
	io_out(ATA_REG_LBA1, (uint8_t)(lba << 8));
    io_out(ATA_REG_LBA2, (uint8_t)(lba << 16));
}

void wait_busy(void) {
    uint8_t status = io_in(ATA_REG_STATUS);
    while (status & ATA_SR_BSY)
        status = io_in(ATA_REG_STATUS);
}

uint8_t poll_command(void) {
    io_in(ATA_REG_STATUS);
    io_in(ATA_REG_STATUS);
    io_in(ATA_REG_STATUS);
    io_in(ATA_REG_STATUS);

    wait_busy();

    uint8_t status;

retry:
    status = io_in(ATA_REG_STATUS);

    if (status & ATA_SR_ERR)
        return 2;
    if ((status & ATA_SR_DRQ))
        goto retry;

    return OS_SUCCESS;
}

void ide_write_block(unsigned lba, uint16_t sector_count, const uint8_t *data) {
    if (lba < 1) {
        printf("IDE write error: LBA < 1\n");
        return;
    }

    current_lba = lba;

    wait_busy();

    io_out(ATA_REG_CONTROL, 0x02);
    ide_select_drive(lba);
    //clear ATA_REG_ERROR(ATA_FEATURES)
    io_out(ATA_REG_ERROR, 0x00);
    select_sector(lba-1, sector_count);
    io_out(ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

    int i;
    uint16_t cur;
    
    for (i = 0; i < sector_count*(512/2); i++) {
        cur = (data[i*2+1] << 8) | data[i*2];        
        io_outw(ATA_REG_DATA, cur);
    }

    wait_400ns();
    io_out(ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
    wait_busy();
}

void ide_read_block(unsigned lba, uint16_t sector_count, uint8_t *data) {
    if (lba < 1) {
        printf("IDE read error: LBA < 1\n");
        return;
    }
    current_lba = lba;
    uint8_t poll_stat;

    io_out(ATA_REG_CONTROL, 0x02);
    poll_stat = poll_command();
    if (poll_stat != 0) {
        printf("stat read: %d\n", poll_stat);
        //return; // status failed because of error.
    }
    io_out(ATA_REG_ERROR, 0x00);
    io_out(ATA_REG_SECCOUNT0, (uint8_t)(sector_count));
    io_out(ATA_REG_SECCOUNT1, (uint8_t)(sector_count >> 8));
    ide_select_drive(lba);
    // set ATA_REG_ERROR(ATA_FEATURES) to zero in case of an error
    select_sector(lba-1, sector_count);
    io_out(ATA_REG_COMMAND, ATA_CMD_READ_PIO);
    
    poll_stat = poll_command();
    if (poll_stat != 0) {
        printf("stat read: %d\n", poll_stat);
        //return; // status failed because of error.
    }
    
    int i;
    uint16_t cur;
    for (i = 0; i < sector_count*256; i++) {
        cur = io_inw(ATA_REG_DATA);
        *(uint16_t *)(data+i*2) = cur;
        io_out(ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
    }

    poll_stat = poll_command();
    wait_400ns();

    io_out(ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
}

void get_info_substr(uint16_t *buffer, int sindex, int length, char *copy_buffer) {
    int i;
    uint16_t cur;

    for (i = 0; i < length/2; i++) {
        //retrieve ushort from buffer
        cur = buffer[sindex+i];
        //split ushort into 2 bytes, push to string buffer
        copy_buffer[i*2] = ((cur >> 8) & 0xFF);
        copy_buffer[i*2+1] = (cur & 0xFF);
    }
}

void remove_spaces(char *in, int length) {
    char out[128];
    // char cur;
    int i, buf_index = 0;
    for (i = 0; i < length; i++) {
        if (in[i] == ' ')
            continue;
        out[buf_index++] = in[i];
    }
    in = out;
}

bool string_cmp_start(char *buf, const char *cmp) {
    int i;
    char tmp[64];
    for (i = 0; i < strlen(cmp); i++) {
        tmp[i] = cmp[i];
    }
    return (bool)strcmp(tmp, cmp);
}

int ide_identify(uint8_t _bus, uint8_t drive) {
    ide_select_drive(current_lba);

    io_out(ATA_REG_SECCOUNT0, 0);
    io_out(ATA_REG_LBA0, 0);
    io_out(ATA_REG_LBA1, 0);
    io_out(ATA_REG_LBA2, 0);

    io_out(ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    
    uint8_t status = io_in(ATA_REG_STATUS);
    if (!status) {
        return OS_FAILURE;
    } 

    //TODO: replace timeout with PIC timer.
    int timeout = 1000000;
    while ((io_in(ATA_REG_STATUS) & ATA_SR_BSY) != 0 && timeout-- != 0);
    //drive has been busy too long, return as failed attempt
    if (timeout == 0)
        return OS_FAILURE;

    do {
        status = io_in(ATA_REG_STATUS);
        if (status & ATA_SR_ERR) {
            //this drives error bit is set!
            return OS_FAILURE;
        }
    } while (!(status & ATA_SR_DRQ));

    //device is fully operational, continue
    return OS_SUCCESS;
}

void ide_init_drive(drive_t *drive) {
    uint16_t buf[256];

    //read 256 shorts from IDE device
    int i;
    for (i = 0; i < 256; i++) {
        buf[i] = io_inw(ATA_REG_DATA);
    }

    get_info_substr(buf, 10, 20, drive->serial_number);
    get_info_substr(buf, 23, 8, drive->firmware_revision);
    get_info_substr(buf, 27, 40, drive->model_number);

    remove_spaces(drive->serial_number, 20);
    remove_spaces(drive->firmware_revision, 8);
    remove_spaces(drive->model_number, 40);

    //null terminate model number
    drive->model_number[41] = 0;
    drive->blocks = ((unsigned)buf[61] << 16 | buf[60])-1;

    // if ((buf[83] & 0x400) != 0) {
    //     drive->flags |= IDE_DRV_LBA48;
    //     drive->blocks = ((size_t)buf[103] << 48 | (size_t)buf[102] << 32 |
    //                          (size_t)buf[101] << 16 | buf[100])-1;
    // }
}

void ide_drives_find(drive_t *drive_buf, int *drive_index) {
    int i, j, index = 0;
    drive_t *cur_drive;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++, index++) {
            bus_position = j;
            bus = i;

            ide_select_drive(current_lba);

            if (ide_identify(i, j))
                continue;

            cur_drive = &drive_buf[index];
            cur_drive->flags = 0;
            cur_drive->bus = i;
            cur_drive->bus_position = j;
            cur_drive->flags |= DRIVE_EXISTS;
            ide_init_drive(cur_drive);
        }
    }
}

void ide_set_bus(int _bus, int _bus_position) {
    bus = _bus;
    bus_position = _bus_position;
    ide_select_drive(current_lba);
}
void ide_init(drive_t *drive_buf, int *drive_index) {
    io_out(ATA_REG_CONTROL, 0x02);

    ide_drives_find(drive_buf, drive_index);
}