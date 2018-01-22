#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <kernel/scancodes.h>

#define KBD_NOBLOCK 0x01

#define KBDK_BACKSPACE 8
#define KBDK_LEFT 128
#define KBDK_RIGHT 129
#define KBDK_UP 130
#define KBDK_DOWN 131

void keyboard_init(void);
char getkey(int flags);

#endif