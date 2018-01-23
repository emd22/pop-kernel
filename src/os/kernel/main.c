#if defined(__linux__)
#error "Error: Not compiled using cross-compiler."
#endif

#if !defined(__i386__)
#error "Error: Kernel needs an ix86 system."
#endif

#include <kernel/drivers/keyboard.h> 
#include <kernel/kbootio.h>
#include <osutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void kmain(void) {
	bvga_init();
	keyboard_init();

	char buf[64];

	kinp(buf, KINP_NOECHO);
	printf("[%s]\n", buf);
}