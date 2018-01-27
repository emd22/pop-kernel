#include <kernel/drivers/mbr.h>
#include <kernel/drivers/ata_pio.h>
#include <kernel/debug.h>
#include <osutil.h>
#include <stdio.h>
#include <stdlib.h>

static mbr_t mbr;

const uint8_t template_mbr[]={
    51,192,142,208,188,0,124,142,192,142,216,139,244,191,0,6,
    185,0,1,252,243,165,234,27,6,0,0,184,115,7,232,27,
    1,190,82,7,232,181,0,232,204,0,190,138,7,173,60,255,
    116,115,139,232,191,0,124,128,61,0,116,8,172,174,117,126,
    60,0,117,248,51,192,128,126,0,128,190,190,7,136,4,136,
    68,16,136,68,32,136,68,48,198,70,0,128,116,39,128,62,
    137,7,0,116,32,191,5,0,186,128,0,185,1,0,184,1,
    3,187,0,6,205,19,115,13,51,192,205,19,79,117,239,190,
    102,7,232,87,0,191,5,0,139,86,0,139,78,2,184,1,
    2,187,0,124,205,19,115,48,51,192,205,19,79,117,239,190,
    102,7,232,55,0,190,89,7,232,49,0,190,138,7,173,60,
    255,117,3,233,107,255,232,35,0,232,38,0,235,240,60,0,
    117,3,233,104,255,172,235,246,196,6,129,7,232,109,0,139,
    245,234,0,124,0,0,190,79,7,232,13,0,172,60,0,117,
    248,195,176,13,232,2,0,176,10,180,14,187,7,0,205,16,
    195,50,228,205,22,195,191,0,124,161,135,7,131,248,0,116,
    26,163,133,7,131,62,133,7,0,117,10,191,0,124,232,209,
    255,198,5,0,195,180,1,205,22,116,233,232,211,255,60,13,
    116,236,60,8,117,12,129,255,0,124,116,205,79,232,166,255,
    235,199,60,32,114,195,170,232,175,255,235,189,250,135,6,112,
    0,163,129,7,140,192,135,6,114,0,163,131,7,251,195,32,
    8,0,10,109,98,114,58,32,0,10,99,104,111,111,115,101,
    58,32,13,10,10,0,100,105,115,107,32,101,114,114,111,114,
    13,10,0,46,131,62,133,7,0,116,5,46,255,14,133,7,
    234,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,85,170,
};

uint32_t to_uint32(uint8_t *n, unsigned pos) {
    return (unsigned)n[pos+3] << 24 | (unsigned)n[pos+2] << 16 |
           (unsigned)n[pos+1] << 8  | n[pos];
}

int get_part_entry(int n) {
    return 446+(n*16);
}

void mbr_init(void) {
    int i;
    partition_t *part;
    
    for (i = 0; i < 4; i++) {
        part = &mbr.partitions[i];
        part->error = OS_NULL;
        part->lba_first_sector = OS_NULL;
        part->lba_sector_count = OS_NULL;
        part->lba_end_sector   = OS_NULL;
    }
}

bool parse_partition(int part, uint8_t *mbr_dat, unsigned location) {
    partition_t *cpart = &mbr.partitions[part];
    char sys_id = mbr_dat[location+4];

    printf("partition %d sysid=%x, lba_start: %d, lba_end: %d\n", part, sys_id, cpart->lba_first_sector, cpart->lba_end_sector);

    if (sys_id == OS_NULL) {
        cpart->error |= EMPTY_PARTITION;
        return true;
    }
    else if (sys_id == 0x05 || sys_id == 0x0F || sys_id == 0x85) {
        cpart->error |= FEATURE_NOT_EXIST;
        return true;
    }
    else if (sys_id == FAT32_SYSID || sys_id == 0x0B) {
        //sys_id is fat32.
        cpart->type = sys_id;
        cpart->lba_first_sector = to_uint32(mbr_dat, location+8);
        cpart->lba_sector_count = to_uint32(mbr_dat, location+12);
        cpart->lba_end_sector = cpart->lba_first_sector+cpart->lba_sector_count;

        cpart->bootable = (mbr_dat[location] == 0x81);
        return false;
    }
    else {
        return true;
    }
}

void format_mbr(void) {
    char confirmation[16];

    printf("Enter 'format' to format MBR: ");
    input(confirmation);
    printf("\n");

    if (strcmp(confirmation, "format")) return;

    printf("Formatting MBR...\n");
    ata_pio_write(0, template_mbr, 512);
    int i;
    for (i = 0; i < 4; i++) {
        mbr_write_part(get_part_entry(i), 0, 0, 0);
    }
    printf("MBR formatted.\n");

}

void mbr_purge() {
    uint8_t dat[512];
    bzero(dat, 512);
    ata_pio_write(0, dat, 512);
}

void mbr_write_part(unsigned loc, uint32_t sp, uint32_t ep, uint8_t sys_id) {
    uint8_t *sp_split = (uint8_t *)&sp;
    uint32_t sc = ep-sp;
    uint8_t *sc_split = (uint8_t *)&(sc);

    ata_pio_write(loc+8, sp_split, 4);
    ata_pio_write(loc+12, sc_split, 4);

    ata_pio_write(loc, &sys_id, 1);
}

bool retrieve_partitions(void) {
    uint8_t *mbr_dat = (uint8_t *)malloc(512);

    ata_pio_read(0, mbr_dat);

    if (mbr_dat[510] != 0x55 || mbr_dat[511] != 0xAA) {
        debug_err("Drive does not have MBR installed.\n");
        return true;
    }

    parse_partition(0, mbr_dat, get_part_entry(0));
    parse_partition(1, mbr_dat, get_part_entry(1));
    parse_partition(2, mbr_dat, get_part_entry(2));
    parse_partition(3, mbr_dat, get_part_entry(3));

    free(mbr_dat);
    return false;
}

mbr_t *get_mbr(void) {
    return &mbr;
}