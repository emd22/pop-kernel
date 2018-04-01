#ifndef MBR_H
#define MBR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define EMPTY_PARTITION 0x01
#define FEATURE_NOT_EXIST 0x02

#define FAT32_SYSID 0x0C

typedef enum {
    NONE,
    PARTITION_1,
    PARTITION_2,
    PARTITION_3,
    PARTITION_4
} PARTITION;

typedef struct {
    int error;
    bool bootable;
    uint8_t type;
    size_t lba_first_sector;
    size_t lba_sector_count;
    size_t lba_end_sector;
} partition_t;

typedef struct {
    partition_t partitions[4];
    uint8_t     signature[2];
} mbr_t;

void mbr_init(void);
bool retrieve_partitions(void);
mbr_t *get_mbr();
void mbr_write_part(unsigned loc, uint32_t sp, uint32_t ep, uint8_t sys_id);
bool format_mbr(void);
int get_part_entry(int n);
void mbr_purge();

#endif