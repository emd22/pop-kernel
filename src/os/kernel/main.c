#if defined(__linux__)
#error "Error: Not compiled using cross-compiler."
#endif

#if !defined(__i386__)
#error "Error: Kernel needs an ix86 system."
#endif

#include <kernel/drivers/keyboard.h> 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void kmain(void) {
	bvga_init();
	keyboard_init();

	char buf[64];

	while (true) {
		if (getkey(KBD_NOBLOCK) != NULL) {

		}
	}
	// bvga_set_colour(bvga_get_colour(BVGA_LIGHT_GREEN, BVGA_GREEN));
	// printf("this is\na test");
	// bvga_set_colour(BVGA_DEF);

	
}