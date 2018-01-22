#include <kernel/boot_vga.h> 
#include <stdio.h>
#include <stdlib.h>

#if defined(__linux__)
#error "Error: Not compiled using cross-compiler."
#endif

#if !defined(__i386__)
#error "Error: Kernel needs an ix86 system."
#endif

void kmain(void) {
	bvga_init();

	bvga_set_colour(bvga_get_colour(BVGA_LIGHT_GREEN, BVGA_GREEN));
	printf("this is\na test");
	bvga_set_colour(BVGA_DEF);
}