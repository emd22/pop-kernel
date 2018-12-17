// #include <kernel/fs/fat32.h>
// #include <kernel/hd.h>
// #include <kernel/drivers/mbr.h>
// #include <stdint.h>
// #include <string.h>

// struct fat32_bpb {
//     uint8_t boot_jmp[3];
//     uint8_t oem[8];
//     uint16_t sector_size;
//     uint8_t  sectors_per_cluster;
//     uint16_t reserved_sectors;
//     uint8_t  num_fats;
//     uint16_t root_entries;
//     uint16_t total_sectors0;
//     uint8_t  media_type;
//     uint16_t sectors_per_fat;
//     uint16_t sectors_per_track;
//     uint16_t head_count;
//     uint32_t start_lba;
//     uint32_t total_sectors1;
// } __attribute__ ((packed));

// struct fat32_ebpb {
//     struct fat32_bpb bpb;
//     uint32_t sectors_per_fat;
//     uint16_t flags;
//     uint16_t fat_version;
//     uint32_t root_clus_number;
//     uint16_t fs_inf_sector_number;
//     uint16_t backup_boot_sector_number;
//     uint8_t  reserved0[12];
//     uint8_t  drive_number;
//     uint8_t  reserved1;
//     uint8_t  signature;
//     uint32_t volume_id;
//     uint8_t  volume_str[11];
//     uint8_t  sys_ident[8];
// } __attribute__ ((packed));

// void set_str(uint8_t *out, uint8_t *in, int start_idx, int str_size) {
//     int i;
//     for (i = 0; i < str_size; i++) {
//         out[start_idx+i] = in[i];
//     }
// }

// void fat32_write(struct fat32_ebpb *ebpb) {
//     struct fa
//     strcpy(bpb->oem, "MSWIN4.1");
//     //TODO: add support for different sector sizes.
//     bpb->sector_size = 512;
//     bpb->sectors_per_cluster = 4;
//     bpb->reserved_sectors = 10;
//     bpb->num_fats = 2;
//     bpb->
// }

// void fat32_init() {
//     uint8_t boot_sector[512];
//     hd_read_block(1, 1, boot_sector);

//     struct fat32_ebpb *ebpb;
    
//     ebpb = *((struct fat32_ebpb *)(boot_sector));

//     if (ext_bpb->signature != 0x28 || ext_bpb->signature != 0x29) {
//         fat32_write(ebpb);
//     }
// }


#include <kernel/fs/fat.h>
#include <kernel/hd.h>
#include <kernel/drivers/ide.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define IS_FILE 0
#define IS_DIR  1

#define FAT16_TOTAL_SECTORS   (bpb->total_sectors0 ? bpb->total_sectors1 : bpb->total_sectors0)
#define FAT16_FAT_SIZE        (bpb->sectors_per_fat)
#define FAT16_ROOT_DIR_SIZE   (((bpb->root_entries*32)+(bpb->sector_size-1))/bpb->sector_size)
#define FAT16_DATA_SECTOR     (bpb->reserved_sectors+(bpb->num_fats*FAT16_FAT_SIZE)+FAT16_ROOT_DIR_SIZE)
#define FAT16_DATA_SECTOR_AMT (FAT16_TOTAL_SECTORS \
                                -(bpb->reserved_sectors\
                                +(bpb->num_fats*FAT16_FAT_SIZE)\
                                +FAT16_ROOT_DIR_SIZE) \
                              )
#define FAT16_ROOT_SECTOR     (FAT16_DATA_SECTOR-FAT16_ROOT_DIR_SIZE)
#define GET_FIRST_SECTOR_OF_CLUSTER(cluster) (((cluster-2)*bpb->sectors_per_cluster)+FAT16_DATA_SECTOR)
#define FAT16_ENTRIES_PER_SECTOR 32
#define FAT16_IS_FILE(entry) (entry->attributes ? IS_DIR : IS_FILE)
#define ENTRY_EMPTY(entry) (!entry->filename[0])

typedef struct {
    uint32_t total_sectors;
    uint16_t reserved_sectors;
    uint8_t bit_size;
} fat_settings_t;

typedef struct {
    uint8_t  boot_jmp[3];
    uint8_t  oem_name[8];
    uint16_t sector_size;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  num_fats;
    uint16_t root_entries;
    uint16_t total_sectors0;
    uint8_t  media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t start_lba;
    uint32_t total_sectors1;
} __attribute__ ((packed)) fat_bpb_t;

typedef struct {
    uint8_t drive_number;
    uint8_t reserved0;
    uint8_t signature;
    uint8_t volume_id[4];
    uint8_t volume_label[11];
    uint8_t sys_ident[8];
} __attribute__ ((packed)) fat16_ebpb_t;

typedef struct {
    uint8_t  filename[11];
    uint8_t  attributes;
    uint8_t  reserved0;
    uint8_t  create_tenths;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t access_date;
    uint16_t cluster_hi;
    uint16_t mod_time;
    uint16_t mod_date;
    uint16_t cluster_lo;
    uint32_t file_size;
} __attribute__ ((packed)) dir_entry_t;

static bool mounted = false;
static fat_bpb_t *bpb;

void fat16_write_ebpb(fat16_ebpb_t *ebpb) {
    ebpb->drive_number = 0x80;
    ebpb->reserved0 = 0;
    ebpb->signature = 0x29;
    memset(ebpb->volume_id, 0, 4);
    memcpy(ebpb->volume_label, "NO NAME    ", 11);
    memcpy(ebpb->sys_ident, "FAT16   ", 8);
}

void fat16_write_tables(fat_settings_t *fat_settings) {
    uint8_t boot_sector[512];
    // hd_read_block(1, 1, boot_sector);
    ide_read_block(0, 1, boot_sector);

    bpb = ((fat_bpb_t *)boot_sector);

    const uint8_t boot_jmp[] = {0xEB, 0x3C, 0x90};

    memcpy(bpb->boot_jmp, boot_jmp, 3);
    memcpy(bpb->oem_name, "T3OS 1.0", 8);

    bpb->sector_size = 512;
    bpb->sectors_per_cluster = 32;
    bpb->reserved_sectors = fat_settings->reserved_sectors;
    bpb->num_fats = 2;
    bpb->root_entries = 512;

    if (fat_settings->total_sectors > (2^16)) {
        bpb->total_sectors0 = 0;
        bpb->total_sectors1 = fat_settings->total_sectors;
    }
    else {
        bpb->total_sectors0 = fat_settings->total_sectors;
    }

    bpb->media_type = 0xF8;
    bpb->sectors_per_fat = 32;
    bpb->sectors_per_track = 32;
    bpb->head_count = 64;
    bpb->start_lba = 0;

    if (fat_settings->bit_size == 12 || fat_settings->bit_size == 16) {
        fat16_ebpb_t *fat16_ebpb = ((fat16_ebpb_t *)(boot_sector+36));
        fat16_write_ebpb(fat16_ebpb);
    }

    boot_sector[510] = 0x55;
    boot_sector[511] = 0xAA;

    ide_write_block(0, 1, boot_sector);
}

void fat16_format(void) {
    fat_settings_t fat_settings;
    fat_settings.total_sectors = 65536;
    fat_settings.reserved_sectors = 1;
    fat_settings.bit_size = 16;

    fat16_write_tables(&fat_settings);
}

uint8_t *fat16_read_file(dir_entry_t *dir_entry) {
    uint8_t *buffer = (uint8_t *)malloc(dir_entry->file_size);
    unsigned sec_start = GET_FIRST_SECTOR_OF_CLUSTER(dir_entry->cluster_lo);
    unsigned sec_count = (dir_entry->file_size)/512;
    if ((dir_entry->file_size) % 512 > 0) {
        sec_count++;
    }

    printf("secc: %d\n", sec_count);
    ide_read_block(sec_start, sec_count, buffer);

    return buffer;
}

int fat16_find_free_dir_entry(dir_entry_t *last_dir) {
    dir_entry_t dir_cluster[FAT16_ENTRIES_PER_SECTOR];
    memset(dir_cluster, 0, sizeof(dir_entry_t)*FAT16_ENTRIES_PER_SECTOR);
    ide_read_block(FAT16_ROOT_SECTOR, 1, (uint8_t *)dir_cluster);
    int i;
    dir_entry_t *this_dir;

    for (i = 0; i < 32; i++) {
        this_dir = &dir_cluster[i];
        if (ENTRY_EMPTY(this_dir)) {
            if (last_dir != NULL)
                last_dir = this_dir;
            return i;
        }
    }
    last_dir = NULL;
    return -1;
}

void fat16_make_dir(char *fn) {
    dir_entry_t *last_dir;
    int file_i = fat16_find_free_dir_entry(last_dir);
    if (file_i == -1) {
        printf("Could not find free entry.\n");
        return;
    }
    dir_entry_t dir;
    strcpy(dir.filename, fn);

    unsigned sec_count = (last_dir->file_size)/512;
    if ((last_dir->file_size) % 512 > 0) {
        sec_count++;
    }

    dir.cluster_lo = sec_count+last_dir->cluster_lo;
}

void fat16_list_dir(void) {
    // 32 entries per sector (512/sizeof(dir_entry_t)) = 32
    dir_entry_t dir_cluster[FAT16_ENTRIES_PER_SECTOR];

    memset(dir_cluster, 0, sizeof(dir_entry_t)*FAT16_ENTRIES_PER_SECTOR);
    ide_read_block(FAT16_ROOT_SECTOR, 1, (uint8_t *)dir_cluster);

    int i;
    dir_entry_t *this_dir;

    for (i = 0; i < 32; i++) {
        this_dir = &dir_cluster[i];
        if (ENTRY_EMPTY(this_dir)) {
            char real_fn[12];
            strcpy(real_fn, (char *)(this_dir->filename));
            real_fn[11] = 0;
            printf("%s - %d : 0x%x\n", real_fn, this_dir->cluster_lo, (GET_FIRST_SECTOR_OF_CLUSTER(this_dir->cluster_lo))*512);
        }
    }
}

void fat16_init(void) {
    uint8_t boot_sector[512];
    ide_read_block(0, 1, boot_sector);
    bpb = ((fat_bpb_t *)boot_sector);
    fat16_ebpb_t *fat16_ebpb = ((fat16_ebpb_t *)(boot_sector+36));

    if (boot_sector[510] != 0x55 || boot_sector[511] != 0xAA) {
        printf("Boot sector formatted as FAT16\n");        
        bpb = ((fat_bpb_t *)boot_sector);
        fat16_ebpb = ((fat16_ebpb_t *)(boot_sector+36));
        fat16_format();
    }

    if (fat16_ebpb->signature == 0x29 || fat16_ebpb->signature == 0x28) {
        mounted = true;
    }
    else {
        printf("Boot sector formatted as FAT16\n");
        bpb = ((fat_bpb_t *)boot_sector);
        fat16_ebpb = ((fat16_ebpb_t *)(boot_sector+36));
        fat16_format();
        bpb = ((fat_bpb_t *)boot_sector);
        fat16_ebpb = ((fat16_ebpb_t *)(boot_sector+36));
    }
    if (mounted)
        fat16_list_dir();
}