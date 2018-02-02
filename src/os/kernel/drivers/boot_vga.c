#include <kernel/drivers/boot_vga.h>
#include <kernel/x86.h>
#include <string.h>

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

uint8_t bvga_no_colour() {
    return tinfo.colour;
}

uint16_t bvga_entry(uint8_t uc, uint8_t colour) {
	return (uint16_t) uc | (uint16_t) colour << 8;
}

void bvga_set_colour(uint8_t colour) {
    tinfo.colour = colour;
}

void bvga_clear(void) {
    tinfo.col = 0;
    tinfo.row = 0;
    bvga_cursorpos(0, 0);
    int i;
    for (i = 0; i < (BVGA_WIDTH*BVGA_HEIGHT)-1; i++) {
        bvga_put(' ', BVGA_DEF, 0);
    }
    tinfo.col = 0;
    tinfo.row = 0;
    bvga_cursorpos(0, 0);
}

void bvga_nl(void) {
    tinfo.col = 0;
    if (++(tinfo.row) == BVGA_HEIGHT) {
        tinfo.row = 0;
        bvga_clear();
        //TODO: boot message scrolling
    }
}

void inc_x(void) {
    if (++(tinfo.col) == BVGA_WIDTH) {
        tinfo.col = 0;
        bvga_nl();
    }
}

bool bvga_set_pos(int x, int y) {
    if ((x <= BVGA_WIDTH && x >= 0) &&
        (y <= BVGA_HEIGHT && y >= 0)) {
            tinfo.col = x;
            tinfo.row = y;
            return true;
    }
    return false;
}

void bvga_get_pos(int *pos) {
    pos[0] = tinfo.col;
    pos[1] = tinfo.row;
}

void bvga_mov_cur(int x_rel, int y_rel) {
    tinfo.col += x_rel;
    tinfo.row += y_rel;
    bvga_cursorpos(tinfo.col, tinfo.row);
}

void bvga_put(char c, uint8_t colour, int flags) {
    const size_t index = tinfo.row * BVGA_WIDTH + tinfo.col;
    tinfo.buffer[index] = bvga_entry(c, colour);

    if (!(flags & BVGA_NOMOVE)) {
        inc_x();
    }
    bvga_cursorpos(tinfo.col, tinfo.row);    
}

void bvga_putstr(const char *str, uint8_t colour) {
    uint8_t old_colour = tinfo.colour;

    int i;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == '\n') {
            bvga_nl();
            continue;
        }
        bvga_put(str[i], colour, 0);
    }

    tinfo.colour = old_colour;
}

void bvga_init(void) {
    tinfo.buffer = BVGA_MEM_ADDR;
    tinfo.col = 0;
    tinfo.row = 0;
    bvga_set_colour(bvga_get_colour(BVGA_LIGHT_GREY, BVGA_BLACK));
}

void bvga_cursorpos(int x, int y) {
    unsigned short position = (y * BVGA_WIDTH) + x;

    // cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position & 0xFF));
    // cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((position >> 8) & 0xFF));
}