#include <kernel/boot_vga.h>

#include <string.h>
#include <stdint.h>

#define BVGA_MEM_ADDR (uint16_t *)0xB8000

struct {
    size_t col;
    size_t row;
    uint8_t colour;
    uint16_t *buffer;
} tinfo;

uint8_t bvga_get_colour(enum BVGA_C fg, enum BVGA_C bg) {
    return fg | bg << 4;
}

uint16_t bvga_entry(uint8_t uc, uint8_t colour) {
	return (uint16_t) uc | (uint16_t) colour << 8;
}

void bvga_set_colour(uint8_t colour) {
    tinfo.colour = colour;
}

void inc_x() {
    if (++(tinfo.col) == BVGA_WIDTH) {
        tinfo.col = 0;
        if (++(tinfo.row) == BVGA_HEIGHT) {
            tinfo.row = 0;
            //TODO: boot message scrolling
        }
    }
}

void bvga_put(char c, uint8_t colour) {
    const size_t index = tinfo.row * BVGA_WIDTH + tinfo.col;
    tinfo.buffer[index] = bvga_entry(c, colour);
    inc_x();
}

void bvga_putstr(const char *str, uint8_t colour) {
    uint8_t old_colour = tinfo.colour;

    int i;
    for (i = 0; i < strlen(str); i++) {
        bvga_put(str[i], colour);
    }

    tinfo.colour = old_colour;
}

void bvga_init(void) {
    tinfo.buffer = BVGA_MEM_ADDR;
    tinfo.col = 0;
    tinfo.row = 0;
    bvga_set_colour(bvga_get_colour(BVGA_LIGHT_GREY, BVGA_BLACK));
}