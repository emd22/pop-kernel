#ifndef MBR_H
#define MBR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define EMPTY_PARTITION 0x01
#define FEATURE_NOT_EXIST 0x02

#define GET_PART_ENTRY(n) (0x1BE+(n*16))

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

#endif