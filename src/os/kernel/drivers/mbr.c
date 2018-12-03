#include <kernel/drivers/mbr.h>
#include <kernel/drivers/ide.h>
#include <kernel/hd.h>
#include <kernel/drive.h>
#include <osutil.h>
#include <stdio.h>
#include <stdlib.h>

static mbr_t mbr;
static drive_t *current_drive;

uint32_t to_uint32(uint8_t *n, unsigned pos) {
    return (unsigned)n[pos+3] << 24 | (unsigned)n[pos+2] << 16 |
           (unsigned)n[pos+1] << 8  | n[pos];
}

int mbr_get_part_entry(int n) {
    return 446+(n*16);
}

bool parse_partition(int part, uint8_t *mbr_dat, unsigned location) {
    partition_t *cpart = &mbr.partitions[part];
    char sys_id = mbr_dat[location+4];

    printf("partition %d sysid=%x, lba_start: %d, lba_end: %d\n", part, sys_id, cpart->lba_first_sector, cpart->lba_end_sector);

    switch (sys_id) {
        case 0:
            cpart->error |= EMPTY_PARTITION;
            return OS_FAILURE;
        case 0x05:
        case 0x0F:
        case 0x0C:
        case 0x0B:
            cpart->type = sys_id;
            cpart->lba_first_sector = to_uint32(mbr_dat, location+8);
            cpart->lba_sector_count = to_uint32(mbr_dat, location+12);
            cpart->lba_end_sector = cpart->lba_first_sector+cpart->lba_sector_count;

            cpart->bootable = (mbr_dat[location] == 0x80);
            return OS_SUCCESS;
        default:
            return OS_FAILURE;
    }
}

void mbr_debug_print(void) {
    int i;
    partition_t *part;
    for (i = 0; i < 4; i++) {
        part = &mbr.partitions[i];
        printf("part #%d = start: %d, end: %d, bootable: %d\n", i, part->lba_first_sector, part->lba_end_sector, part->bootable);
    }
}

void mbr_write_part(unsigned loc, uint32_t start, uint32_t end, uint8_t sys_id) {
    uint32_t sector_count = end-start;

    uint8_t mbr[512];
    ide_read_block(1, 1, mbr);

    mbr[loc+8] = (uint8_t)start;
    mbr[loc+9] = (uint8_t)(start << 8);
    mbr[loc+10] = (uint8_t)(start << 16);
    mbr[loc+11] = (uint8_t)(start << 24);

    mbr[loc+12] = (uint8_t)(sector_count);
    mbr[loc+13] = (uint8_t)(sector_count << 8);
    mbr[loc+14] = (uint8_t)(sector_count << 16);
    mbr[loc+15] = (uint8_t)(sector_count << 24);

    mbr[loc] = sys_id;

    ide_write_block(1, 1, mbr);
}

mbr_t *get_mbr(void) {
    return &mbr;
}

bool mbr_retrieve_partitions(void) {
    uint8_t *mbr_dat = (uint8_t *)malloc(512);

    ide_read_block(1, 1, mbr_dat);

    if (mbr_dat[510] != 0x55 || mbr_dat[511] != 0xAA) {
        return OS_FAILURE;
    }

    parse_partition(0, mbr_dat, mbr_get_part_entry(0));
    parse_partition(1, mbr_dat, mbr_get_part_entry(1));
    parse_partition(2, mbr_dat, mbr_get_part_entry(2));
    parse_partition(3, mbr_dat, mbr_get_part_entry(3));

    free(mbr_dat);
    return OS_SUCCESS;
}

void mbr_init(drive_t *_current_drive) {
    current_drive = _current_drive;

    int i;
    partition_t *part;
    
    for (i = 0; i < 4; i++) {
        part = &mbr.partitions[i];
        memset(part, 0, sizeof(partition_t));
    }
    mbr_retrieve_partitions();
}