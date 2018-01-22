#include <kernel/keyboard.h>

#include <stdint.h>
#include <stdbool.h>

#define NULL 0
#define KBD_PORT 0x64

struct {
    uint8_t *key_cache;
    uint8_t  last_key;
    uint16_t key_loc;
    bool     enabled;
} kinfo;

void keyboard_init() {
    kinfo.key_cache = (uint8_t *)malloc(256);
    kinfo.last_key  = NULL;
    kinfo.key_loc   = NULL;
    kinfo.enabled   = true;
    
    bzero(kinfo.key_cache, 256);
}

char get_scancode() {
    char flag = inb(KBD_PORT);

    while(!(flag & 1)) {
        flag = inb(KBD_PORT);
    }
    return inb(0x60);
}

// char getchar() {
//     return scancodes[get_scancode()+1];
// }