#if defined(__linux__)
#error "Error: Not compiled using cross-compiler."
#endif

#if !defined(__i386__)
#error "Error: Kernel needs an ix86 system."
#endif

#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/idt.h>
#include <kernel/drivers/ata_pio.h>
#include <osutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

void kmain(void) {
	bvga_init();
	keyboard_init();
	idt_install();
	ata_pio_install();
	printf("Made it here\n");

	uint8_t message[] = {'H', 'e', 'l', 'l', 'o', '!', '\0'};

	ata_pio_write(1000, message, 7);

	uint8_t *buf = (uint8_t *)malloc(512);

	ata_pio_read(1000, buf);

	// printf("dat:%s\n", buf);
	int i = 0;
	while (i < 7) {
		printf("[%c]\n", buf[i++]);
	}

	free(buf);
}