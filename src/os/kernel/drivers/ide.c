#include <kernel/drivers/ide.h>
#include <kernel/drivers/pata.h>
#include <kernel/drivers/pci.h>
#include <kernel/arch_io.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define ATA_SR_BSY  0x80
#define ATA_SR_DRDY 0x40
#define ATA_SR_DF   0x20
#define ATA_SR_DSC  0x10
#define ATA_SR_DRQ  0x08
#define ATA_SR_CORR 0x04
#define ATA_SR_IDX  0x02
#define ATA_SR_ERR  0x01

#define ATA_ER_BBK   0x80
#define ATA_ER_UNC   0x40
#define ATA_ER_MC    0x20
#define ATA_ER_IDNF  0x10
#define ATA_ER_MCR   0x08
#define ATA_ER_ABRT  0x04
#define ATA_ER_TK0NF 0x02
#define ATA_ER_AMNF  0x01

#define ATA_CMD_READ_PIO        0x20
#define ATA_CMD_READ_PIO_EXT    0x24
#define ATA_CMD_READ_DMA        0xC8
#define ATA_CMD_READ_DMA_EXT    0x25
#define ATA_CMD_WRITE_PIO       0x30
#define ATA_CMD_WRITE_PIO_EXT   0x34
#define ATA_CMD_WRITE_DMA       0xCA
#define ATA_CMD_WRITE_DMA_EXT   0x35
#define ATA_CMD_CACHE_FLUSH     0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET          0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY        0xEC

#define ATAPI_CMD_READ  0xA8
#define ATAPI_CMD_EJECT 0x1B

#define ATA_MASTER 0x00
#define ATA_SLAVE  0x01

#define ATA_PRIMARY   0x00
#define ATA_SECONDARY 0x01

#define IDE_ATA   0x00
#define IDE_ATAPI 0x01

#define ATA_REG_DATA         0x00
#define ATA_REG_ERROR        0x01
#define ATA_REG_FEATURES     0x01
#define ATA_REG_SECCOUNT0    0x02
#define ATA_REG_LBA0         0x03
#define ATA_REG_LBA1         0x04
#define ATA_REG_LBA2         0x05
#define ATA_REG_DRIVE_SELECT 0x06
#define ATA_REG_COMMAND      0x07
#define ATA_REG_STATUS       0x07
#define ATA_REG_SECCOUNT1    0x08
#define ATA_REG_LBA3         0x09
#define ATA_REG_LBA4         0x0A
#define ATA_REG_LBA5         0x0B
#define ATA_REG_CONTROL      0x0C
#define ATA_REG_ALTSTATUS    0x0C
#define ATA_REG_DEVADDRESS   0x0D

#define ATA_DRV_DEFAULT 0xA0
#define ATA_DRV_LBA     0x40
#define ATA_DRV_SLAVE   0x10

#define ATA_DRV_NULL   -1
#define ATA_DRV_PATA   0x0000
#define ATA_DRV_SATA   0xC33C
#define ATA_DRV_PATAPI 0xEB14
#define ATA_DRV_SATAPI 0x9669

#define ATA_PRIMARY_IO   0x1F0
#define ATA_SECONDARY_IO 0x170

#define ATA_PRIMARY   0
#define ATA_SECONDARY 1

#define GET_IO_BAR0 (bus == ATA_PRIMARY ? ATA_PRIMARY_IO : ATA_SECONDARY_IO)
#define GET_IO_BAR1 (bus == ATA_PRIMARY ? 0x3F4 : 0x374)

struct ide_channel_regs {
    uint16_t base;
    uint16_t control;
    uint16_t bus_master_ide;
    uint8_t no_interrupt;
} channels[2];

struct ide_device {
    uint8_t exists;
    uint8_t channel; //primary or secondary channel(0 or 1)
    uint8_t drive; //master or slave (0 or 1)
    uint8_t type; //ata or atapi (0, 1)
    uint16_t signature;
    uint16_t features;
    unsigned command_sets;
    unsigned size; //size in sectors
    uint8_t model[41];
} ide_devices[4];

static uint8_t irq_invoked = 0;
static uint8_t atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int max_write_pio = 0;

static uint8_t bus_position = ATA_MASTER;
static uint8_t bus = ATA_PRIMARY;

void io_out(uint16_t port, uint8_t val) {
    int io = GET_IO_BAR0;
    if (port == ATA_REG_CONTROL)
        io = GET_IO_BAR1;
    outb(port, val);
}

void io_outw(uint16_t port, uint16_t val) {
    int io = GET_IO_BAR0;
    if (port == ATA_REG_CONTROL)
        io = GET_IO_BAR1;
    outw(port, val);
}

uint16_t io_inw(uint16_t port) {
    int io = GET_IO_BAR0;
    if (port == ATA_REG_CONTROL)
        io = GET_IO_BAR1;
    return inw(io+port);
}

uint8_t io_in(uint16_t port) {
    int io = GET_IO_BAR0;
    if (port == ATA_REG_CONTROL)
        io = GET_IO_BAR1;
    return inb(io+port);
}

void wait_400ns() {
    io_in(ATA_REG_STATUS);
    io_in(ATA_REG_STATUS);
    io_in(ATA_REG_STATUS);
    io_in(ATA_REG_STATUS);
}

void ide_select_drive(uint8_t _bus, uint8_t _bus_position) {
    bus = _bus, bus_position = _bus_position;
    int cmd = bus_position == ATA_MASTER ? 0xA0 : 0xB0;
    io_out(ATA_REG_DRIVE_SELECT, 0xE0 | _bus_position << 4 | 0);
    wait_400ns();
}

void select_sector(size_t sector_pos, unsigned sector_count) {
    io_out(ATA_REG_SECCOUNT0, sector_count);
    io_out(ATA_REG_LBA0, (sector_pos & 0xFF));
    io_out(ATA_REG_LBA1, (sector_pos & 0xFF00) >> 8);
    io_out(ATA_REG_LBA2, (sector_pos & 0xFF0000) >> 16);
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
    char cur;
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

int ide_send_command(uint8_t command) {
    io_out(ATA_REG_COMMAND, command);
    int status, timeout = 20000000;
    do {
        wait_400ns();
        status = io_in(ATA_REG_CONTROL);
    } while ((status & ATA_SR_BSY) != 0 && (status & ATA_SR_ERR) && timeout-- > 0);

    if (!(status & ATA_SR_ERR) || timeout == 0)
        return 1;
    
    return 0;
}

int ide_identify(uint8_t bus, uint8_t drive) {
    ide_select_drive(bus, drive);

    io_out(ATA_REG_SECCOUNT0, 0);
    io_out(ATA_REG_LBA0, 0);
    io_out(ATA_REG_LBA1, 0);
    io_out(ATA_REG_LBA2, 0);

    io_out(ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    
    uint8_t status = io_in(ATA_REG_STATUS);
    if (!status) {
        printf("Drive %d:%d status failed.\n", bus, drive);
        return 0;
    } 

    int timeout = 1000000;
    while (io_in(ATA_REG_STATUS) & ATA_SR_BSY != 0 && timeout-- != 0);
    if (timeout == 0)
        return 0;

    // int io = GET_IO_BAR1;
    do {
        status = io_in(ATA_REG_STATUS);
        printf("STATUS: %d\n", status);
        while (1);
        if (status & ATA_SR_ERR) {
            printf("Drive %d:%d error bit set!\n", bus, drive);
            return 0;
        }
    } while (!(status & ATA_SR_DRQ));

    printf("Drive %d:%d operational!\n", bus, drive);
    return 1;
}

const char *ide_drives_find(int *bus, int *drive) {
    char drives[] = {0, 0, 0, 0};
    int i, j;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (ide_identify(i, j)) {
                drives[i+j] = 1;
                (*bus) = i;
                (*drive) = j;
            }
        }
    }

    return (const char *)drives;
}

// int ide_drive_discover(void) {
//     io_out(ATA_REG_SECCOUNT0, 0);
//     io_out(ATA_REG_LBA0, 0);
//     io_out(ATA_REG_LBA1, 0);
//     io_out(ATA_REG_LBA2, 0);

//     int status = ide_send_command(ATA_CMD_IDENTIFY);

//     if ((status & ATA_SR_ERR) != 0) {
//         uint16_t type_id;
//         type_id = (io_in(ATA_REG_LBA2) << 8 | io_in(ATA_REG_LBA1));
//         if (type_id == ATA_DRV_PATAPI ||
//             type_id == ATA_DRV_SATA ||
//             type_id == ATA_DRV_SATAPI)
//         {
//             return type_id;
//         }
//         return ATA_DRV_NULL;
//     }

//     do {
//         wait_400ns();
//         status = io_in(ATA_REG_STATUS);
//     } while ((status & ATA_SR_DRQ) == 0 && (status & ATA_SR_ERR) == 0);

//     if ((status & ATA_SR_ERR) != 0 || (status & ATA_SR_DRQ) == 0)
//         return ATA_DRV_NULL;

//     return ATA_DRV_PATA;
// }

const char *ide_reset(void) {
    io_out(ATA_REG_CONTROL, 0x04);
    wait_400ns();
    io_out(ATA_REG_CONTROL, 0x00);
    int status;
    int timeout = 20000000;
    do {
        wait_400ns();
        status = io_in(ATA_REG_CONTROL);
    } while ((status & ATA_SR_BSY) != 0 && (status & ATA_SR_ERR) == 0 && timeout-- > 0);
    
    if ((status & ATA_SR_ERR) != 0 || timeout == 0) {
        return timeout == 0 ? "ATA software reset timeout" : "ATA software reset error";
    }

    ide_select_drive(0, 0);

    return NULL;
}

int ide_init_drive(/* int drive_type */) {
    /* if (drive_type == ATA_DRV_PATAPI) {
        const char *err = ide_reset();
        if (err != NULL)
            return -1;
        ide_send_command(ATA_CMD_IDENTIFY_PACKET);
    } */
    uint16_t dev_info_buf[256];

    uint16_t buf[256];

    int i;
    for (i = 0; i < 256; i++) {
        buf[i] = inw(ATA_REG_DATA);
    }

    ide_drive_t ide_drive;

    ide_drive.max_write_pio = 0;

    //copy serial number to drive structure
    get_info_substr(buf, 10, 20, ide_drive.serial_number);
    get_info_substr(buf, 23, 8, ide_drive.firmware_revision);
    get_info_substr(buf, 27, 40, ide_drive.model_number);

    remove_spaces(ide_drive.serial_number, 20);
    remove_spaces(ide_drive.firmware_revision, 8);
    remove_spaces(ide_drive.model_number, 40);

    ide_drive.model_number[41] = 0;

    if (string_cmp_start(ide_drive.model_number, "Hitachi") == 0 || 
        string_cmp_start(ide_drive.model_number, "FUJITSU") == 0)
    {
        max_write_pio = 1;
    }

    ide_drive.blocks = ((unsigned)buf[61] << 16 | buf[60])-1;
    printf("hd: %s\n", ide_drive.model_number);
    
    ide_drive.lba48 = (buf[83] & 0x0400) != 0;

    if (ide_drive.lba48)
        ide_drive.blocks = ((unsigned long)buf[103] << 48 | (unsigned long)buf[102] << 32 |
                           (unsigned long)buf[101] << 16 | buf[100]) - 1;

    return 0;
}

void ide_read_block(size_t block_pos, unsigned block_count, char *data) {
    io_out(1, 0x00);
    io_out(2, 1);
    io_out(ATA_REG_LBA0, (uint8_t)block_pos);
    io_out(ATA_REG_LBA1, (uint8_t)(block_pos >> 8));
    io_out(ATA_REG_LBA2, (uint8_t)(block_pos >> 16));

    uint16_t slave_bit = 0;

    io_out(0x00, 0xE0 | (slave_bit << 4) | ((block_pos >> 24) & 0x0F));

    ide_send_command(ATA_CMD_READ_PIO);
    
    uint16_t cur;
    int i;
    for (i = 0; i < 512/2; i++) {
        cur = io_in(ATA_REG_DATA);
        data[i*2]  = (uint8_t)cur;
        data[i*2+1] = (uint8_t)(cur >> 8);
    }
    io_out(ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
}

void ide_write_block(size_t block_pos, unsigned block_count, const char *data) {
    io_out(1, 0x00);
    io_out(2, 1);
    io_out(ATA_REG_LBA0, (uint8_t)block_pos);
    io_out(ATA_REG_LBA1, (uint8_t)(block_pos >> 8));
    io_out(ATA_REG_LBA2, (uint8_t)(block_pos >> 16));

    uint16_t slave_bit = 0;

    io_out(0x00, 0xE0 | (slave_bit << 4) | ((block_pos >> 24) & 0x0F));

    ide_send_command(ATA_CMD_WRITE_PIO);

    int i;
    uint16_t cur;
    for (i = 0; i < block_count*512/2; i++) {
        cur = (data[i*2+1] << 8) | data[i*2];
        io_out(ATA_REG_DATA, cur);
    }

    io_out(ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
    // ide_send_command(ATA_CMD_CACHE_FLUSH);
}

int ide_init(void) {
    //disable IRQs because we're polling.
    ide_select_drive(0, 0);
    io_out(ATA_REG_CONTROL, 0x02);

    // // int drive_type = ide_drive_discover();
    const char *drives;
    int bus = 0, drive = 0;
    drives = ide_drives_find(&bus, &drive);

    bus = 0;
    drive = 0;

    ide_select_drive(bus, drive);

    // int drive_type = ide_drive_discover();

    // switch (drive_type) {
        // case ATA_DRV_PATAPI:
            // TODO: when SATAPI is implemented, check if PATAPI is the drive and set block size to 2048.
        // case ATA_DRV_PATA:
            if (ide_init_drive(/* drive_type */) == -1) {
                printf("IDE drive init failed.\n");
                return -1;
            }
            // break;
        // case ATA_DRV_NULL:
            // printf("NUll ;(\n");
            // return -1;
        // default:
            // return -1;
    // }
    return 0;
}