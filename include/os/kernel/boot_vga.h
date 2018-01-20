#ifndef BOOT_VGA_H
#define BOOT_VGA_H

#include <stdint.h>
#include <stddef.h>

#define BVGA_WIDTH 80
#define BVGA_HEIGHT 25

enum BVGA_C {
	BVGA_BLACK,
	BVGA_BLUE,
	BVGA_GREEN,
	BVGA_CYAN,
	BVGA_RED,
	BVGA_MAGENTA,
	BVGA_BROWN,
	BVGA_LIGHT_GREY,
	BVGA_DARK_GREY,
	BVGA_LIGHT_BLUE,
	BVGA_LIGHT_GREEN,
	BVGA_LIGHT_CYAN,
	BVGA_LIGHT_RED,
	BVGA_LIGHT_MAGENTA,
	BVGA_LIGHT_BROWN,
	BVGA_WHITE,
};

uint16_t bvga_entry(uint8_t uc, uint8_t color);
uint8_t  bvga_get_colour(enum BVGA_C fg, enum BVGA_C bg);
void     bvga_init(void);
void     bvga_set_colour(uint8_t colour);
void     bvga_putstr(const char *str, uint8_t colour);

#endif