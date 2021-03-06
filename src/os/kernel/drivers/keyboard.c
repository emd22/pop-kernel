#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/irq.h>
#include <kernel/scancodes.h>
#include <kernel/x86.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define KBD_PORT 0x64

struct {
    // uint8_t *key_cache;
    uint8_t  last_key;
    uint16_t key_loc;
    bool     shift;
    bool     enabled;
    bool     irq_state;
} kinfo;

char get_scancode() {
    // char flag = inb(KBD_PORT);

    // while(!(flag & 1)) {
    //     flag = inb(KBD_PORT);
    // }
    if (inb(0x60) != 0)
        return inb(0x60);
    return -1;
}

uint8_t scancode_char(int keycode) {
    bool shifted = kinfo.shift;

    if (keycode != SCANCODE_ERROR) {
        switch (keycode) {
            case SCANCODE_1: return (!shifted) ? '1' : '!';
            case SCANCODE_2: return (!shifted) ? '2' : '@';
            case SCANCODE_3: return (!shifted) ? '3' : '#';
            case SCANCODE_4: return (!shifted) ? '4' : '$';
            case SCANCODE_5: return (!shifted) ? '5' : '%';
            case SCANCODE_6: return (!shifted) ? '6' : '^';
            case SCANCODE_7: return (!shifted) ? '7' : '&';
            case SCANCODE_8: return (!shifted) ? '8' : '*';
            case SCANCODE_9: return (!shifted) ? '9' : '(';
            case SCANCODE_0: return (!shifted) ? '0' : ')';
            case SCANCODE_DASH: return (!shifted) ? '-' : '_';
            case SCANCODE_EQUAL: return (!shifted) ? '=' : '+';
            case SCANCODE_BACKSPACE: return 8;
            case SCANCODE_TAB: return '\t';
            case SCANCODE_Q: return (!shifted) ? 'q' : 'Q';
            case SCANCODE_W: return (!shifted) ? 'w' : 'W';
            case SCANCODE_E: return (!shifted) ? 'e' : 'E';
            case SCANCODE_R: return (!shifted) ? 'r' : 'R';
            case SCANCODE_T: return (!shifted) ? 't' : 'T';
            case SCANCODE_Y: return (!shifted) ? 'y' : 'Y';
            case SCANCODE_U: return (!shifted) ? 'u' : 'U';
            case SCANCODE_I: return (!shifted) ? 'i' : 'I';
            case SCANCODE_O: return (!shifted) ? 'o' : 'O';
            case SCANCODE_P: return (!shifted) ? 'p' : 'P';
            case SCANCODE_LEFT_BRACKET: return (!shifted) ? '[' : '{';
            case SCANCODE_RIGHT_BRACKET: return (!shifted) ? ']' : '}';
            case SCANCODE_RETURN: return '\n';
            case SCANCODE_A: return (!shifted) ? 'a' : 'A';
            case SCANCODE_S: return (!shifted) ? 's' : 'S';
            case SCANCODE_D: return (!shifted) ? 'd' : 'D';
            case SCANCODE_F: return (!shifted) ? 'f' : 'F';
            case SCANCODE_G: return (!shifted) ? 'g' : 'G';
            case SCANCODE_H: return (!shifted) ? 'h' : 'H';
            case SCANCODE_J: return (!shifted) ? 'j' : 'J';
            case SCANCODE_K: return (!shifted) ? 'k' : 'K';
            case SCANCODE_L: return (!shifted) ? 'l' : 'L';
            case SCANCODE_SEMICOLON: return (!shifted) ? ';' : ':';
            case SCANCODE_QUOTE: return (!shifted) ? '\'' : '\"';
            case SCANCODE_BACK_QUOTE: return (!shifted) ? '`' : '~';
            case SCANCODE_BACKSLASH: return (!shifted) ? '\\' : '|';
            case SCANCODE_Z: return (!shifted) ? 'z' : 'Z';
            case SCANCODE_X: return (!shifted) ? 'x' : 'X';
            case SCANCODE_C: return (!shifted) ? 'c' : 'C';
            case SCANCODE_V: return (!shifted) ? 'v' : 'V';
            case SCANCODE_B: return (!shifted) ? 'b' : 'B';
            case SCANCODE_N: return (!shifted) ? 'n' : 'N';
            case SCANCODE_M: return (!shifted) ? 'm' : 'M';
            case SCANCODE_COMMA: return (!shifted) ? ',' : '<';
            case SCANCODE_PERIOD: return (!shifted) ? '.' : '>';
            case SCANCODE_FORWARDSLASH: return (!shifted) ? '/' : '?';
            case SCANCODE_SPACEBAR: return ' ';
            case SCANCODE_LEFT_ARROW: return 128;
            case SCANCODE_RIGHT_ARROW: return 129;
            case SCANCODE_UP_ARROW: return 130;
            case SCANCODE_DOWN_ARROW: return 131;
    
        }
    }
    return 0;
}

void kbd_handler(regs_t *regs) {
    int sc = get_scancode();

    if (sc == -1)
        return;

    // if (sc & 0x80) {
    //     int sc_released = sc-0x80;
    //     // printf("{0x%d}\n", sc_released);
    //     if (sc_released == SCANCODE_LEFT_SHIFT || sc_released == SCANCODE_RIGHT_SHIFT)
    //         kinfo.shift = false;
    // }

    // printf("sc={%d}", sc);
    
    if (sc != 0) {
        if (sc == SCANCODE_LEFT_SHIFT || sc == SCANCODE_RIGHT_SHIFT) {
            kinfo.shift = true;
        }
        else if (sc == -0x56 || sc == -0x4A) {
            kinfo.shift = false;
        }
        else {
            char ch = scancode_char(sc);
            kinfo.last_key = ch;
            kinfo.irq_state = true;
        }
    }
    
    irq_ack(1);
}

void keyboard_init(void) {
    // kinfo.key_cache = (uint8_t *)malloc(256);
    kinfo.last_key  = 0;
    kinfo.key_loc   = 0;
    kinfo.enabled   = true;
    kinfo.shift     = false;
    kinfo.irq_state = false;
    
    // bzero(kinfo.key_cache, 256);

    irq_install_handler(1, kbd_handler);    
}

char getkey(int flags) {
    while (!kinfo.irq_state);
    kinfo.irq_state = false;
    return kinfo.last_key;
}