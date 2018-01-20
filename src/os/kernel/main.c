#include <kernel/boot_vga.h> 

#if defined(__linux__)
#error "Error: Not compiled using cross-compiler."
#endif

#if !defined(__i386__)
#error "Error: Kernel needs an ix86 system."
#endif

void kmain(void) {
	bvga_init();

	bvga_putstr("TestOS!", bvga_get_colour(BVGA_CYAN, BVGA_BLACK));
}