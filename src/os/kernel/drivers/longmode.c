extern uint8_t has_cpuid;
extern uint8_t has_longmode;

extern void longmode_init_asm(void);

void longmode_init() {
    longmode_init_asm();
}
