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
#include <string.h>
#include <stdio.h>

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

static bool mounted = false;

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
    ide_read_block(1, 1, boot_sector);

    fat_bpb_t *fat_bpb = ((fat_bpb_t *)boot_sector);

    const uint8_t boot_jmp[] = {0xEB, 0x3C, 0x90};

    memcpy(fat_bpb->boot_jmp, boot_jmp, 3);
    memcpy(fat_bpb->oem_name, "T3OS 1.0", 8);

    fat_bpb->sector_size = 512;
    fat_bpb->sectors_per_cluster = 32;
    fat_bpb->reserved_sectors = fat_settings->reserved_sectors;
    fat_bpb->num_fats = 2;
    fat_bpb->root_entries = 512;

    if (fat_settings->total_sectors > (2^16)) {
        fat_bpb->total_sectors0 = 0;
        fat_bpb->total_sectors1 = fat_settings->total_sectors;
    }
    else {
        fat_bpb->total_sectors0 = fat_settings->total_sectors;
    }

    fat_bpb->media_type = 0xF8;
    fat_bpb->sectors_per_fat = 32;
    fat_bpb->sectors_per_track = 32;
    fat_bpb->head_count = 64;
    fat_bpb->start_lba = 0;

    if (fat_settings->bit_size == 12) {
        fat16_ebpb_t *fat16_ebpb = ((fat16_ebpb_t *)(boot_sector+36));
        fat16_write_ebpb(fat16_ebpb);
    }

    boot_sector[510] = 0x55;
    boot_sector[511] = 0xAA;

    ide_write_block(1, 1, boot_sector);
}

void fat16_format(void) {
    fat_settings_t fat_settings;
    fat_settings.total_sectors = 65536;
    fat_settings.reserved_sectors = 1;
    fat_settings.bit_size = 12;

    fat16_write_tables(&fat_settings);
}

void fat16_get_root(fat_bpb_t *bpb, fat16_ebpb_t *ebpb) {
    // if total sectors(16 bit) != 0, set to 16 bit entry, else, set to 32 bit entry.
    unsigned total_sectors = (bpb->total_sectors0) ? bpb->total_sectors1 : bpb->total_sectors0;
    unsigned fat_size = bpb->sectors_per_fat;

    unsigned root_dir_size = ((bpb->root_entries*32)+(bpb->sector_size-1))/bpb->sector_size;
    unsigned first_data_sector = bpb->reserved_sectors+(bpb->num_fats*fat_size)+root_dir_size;
    unsigned amt_data_sectors = total_sectors-(bpb->reserved_sectors+(bpb->num_fats*fat_size)+root_dir_size);

    unsigned first_root_sector = first_data_sector-root_dir_size;
}

// void fat16_addr_clusters() {
//     uint8_t fat_table[512];
//     unsigned fat_offset = 
// }

void fat16_init(void) {
    uint8_t boot_sector[512];
    ide_read_block(1, 1, boot_sector);
    fat_bpb_t  *fat16_bpb = ((fat_bpb_t *)boot_sector);
    fat16_ebpb_t *fat16_ebpb = ((fat16_ebpb_t *)(boot_sector+36));

    if (boot_sector[510] != 0x55 || boot_sector[511] != 0xAA) {
        printf("Boot sector formatted as FAT16\n");        
        fat16_format();
    }

    if (fat16_ebpb->signature == 0x29 || fat16_ebpb->signature == 0x28) {
        mounted = true;
        printf("OEM NAME: %s\n", fat16_ebpb->sys_ident);
    }
    else {
        printf("Boot sector formatted as FAT16\n");
        fat16_format();
    }
}