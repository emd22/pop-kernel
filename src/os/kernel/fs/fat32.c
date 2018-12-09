#include <kernel/fs/fat32.h>
#include <kernel/hd.h>
#include <kernel/drivers/mbr.h>
#include <stdint.h>
#include <string.h>

struct fat32_bpb {
    uint8_t boot_jmp[3];
    uint8_t oem[8];
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
} __attribute__ ((packed));

struct fat32_ebpb {
    struct fat32_bpb bpb;
    uint32_t sectors_per_fat;
    uint16_t flags;
    uint16_t fat_version;
    uint32_t root_clus_number;
    uint16_t fs_inf_sector_number;
    uint16_t backup_boot_sector_number;
    uint8_t  reserved0[12];
    uint8_t  drive_number;
    uint8_t  reserved1;
    uint8_t  signature;
    uint32_t volume_id;
    uint8_t  volume_str[11];
    uint8_t  sys_ident[8];
} __attribute__ ((packed));

void set_str(uint8_t *out, uint8_t *in, int start_idx, int str_size) {
    int i;
    for (i = 0; i < str_size; i++) {
        out[start_idx+i] = in[i];
    }
}

void fat32_write(struct fat32_ebpb *ebpb) {
    strcpy(bpb->oem, "MSWIN4.1");
    //TODO: add support for different sector sizes.
    bpb->sector_size = 512;
    bpb->sectors_per_cluster = 4;
    bpb->reserved_sectors = 10;
    bpb->num_fats = 2;
    bpb->
}

void fat32_init() {
    uint8_t boot_sector[512];
    hd_read_block(1, 1, boot_sector);

    struct fat32_ebpb *ebpb;
    
    ebpb = *((struct fat32_ebpb *)(boot_sector));

    if (ext_bpb->signature != 0x28 || ext_bpb->signature != 0x29) {
        fat32_write(ebpb);
    }
}