extern int has_cpuid;
extern int has_longmode;

extern void longmode_init_asm(void);

void longmode_init() {
    longmode_init_asm();
}
