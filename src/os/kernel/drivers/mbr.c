#include <kernel/drivers/mbr.h>
#include <kernel/drivers/ata_pio.h>
#include <osutil.h>

struct {
    partition_t partitions[4];
    uint8_t     signature[2];
} mbr;

uint32_t to_uint32(uint8_t *n, unsigned pos) {
    return (unsigned)n[pos+3] << 24 | (unsigned)n[pos+2] << 16 |
           (unsigned)n[pos+1] << 8  | n[pos];
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

bool parse_partition(int part, uint8_t *mbr_dat, size_t location) {
    partition_t *part = &mbr.partitions[part];
    char sys_id = mbr_dat[location+4];

    printf("partition %d sysid=%x\n", part, sys_id);

    if (sys_id == OS_NULL) {
        part->error |= EMPTY_PARTITION;
        return true;
    }
    else if (sys_id == 0x05 || sys_id == 0x0F || sys_id == 0x85) {
        part->error |= FEATURE_NOT_EXIST;
        return true;
    }
    else if (sys_id == 0x0C || sys_id == 0x0B) {
        //sys_id is fat32.
        part->type = sys_id;
        part->lba_first_sector = to_uint32(mbr_dat, loc+8);
        part->lba_sector_count = to_uint32(mbr_dat, loc+12);
        part->lba_end_sector = part->lba_first_sector+part->lba_sector_count;

        part->bootable = (mbr_dat[loc] == 0x81);
        return false;
    }
    else {
        return true;
    }
}