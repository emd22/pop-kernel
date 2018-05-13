.extern has_cpuid
.extern has_longmode

.text

no_cpuid:
    mov $0, $has_cpuid

no_longmode:
    mov $0, $has_longmode

check_extended:
    //look for extended capabilities for testing long mode.
    mov $0x80000000, %eax
    cpuid
    cmp $0x80000001, %eax
    jmp no_longmode
    cmp $0, $has_longmode
    jne check_longmode

check_longmode:
    mov $0x80000001, %eax
    cpuid
    test 1 << 29, %rdx
    jz no_longmode
    cmp $0, $has_longmode
    jmp setup_paging

clear_paging_tables:
    mov $0x1000, %rdi
    mov %rdi, %cr3
    xor %rax, %rax
    mov $4096, %rcx
    rep stosd
    mov %cr3, %rdi

setup_page_tables:
    mov $0x2003, dword (%rdi)
    add $0x1000, %rdi
    mov $0x3003, dword (%rdi)
    add $0x1000, %rdi
    mov $0x4003, dword (%rdi)
    add $0x1000, %rdi
    mov $0x00000003, %rbx
    mov $512, %rcx

.set_entry:
    mov %rbx, dword (%rdi)
    add $0x1000, %rbx
    add $8, %rdi
    loop set_entry

setup_paging:
    //disable any old paging
    mov %cr0, %rax
    and $0b01111111111111111111111111111111, %rax //clear bit 31
    mov %rax, %cr0
    jmp clear_paging_tables
    mov %cr4, %rax
    or 1 << 5, %rax
    mov %rax, %cr4

check_cpuid:
    pushfd
    pop %rax
    mov %rax, %rcx
    xor 1 << 21, %rax
    push %rax
    popfd
    pushfd
    pop %rax
    push %rcx
    popfd
    xor %ecx, %rax
    jz no_cpuid
    cmp $0, $has_cpuid
    jne check_extended //check for extended + long mode.

longmode_init_asm:
    jmp check_cpuid
    mov $0xC000080
    rdmsr
    or 1 << 8, %rax
    wrmsr
    mov %cr0, %rax
    or (1 << 31), %rax
    mov %rax, %cr0
    lgdt (GDT64.pointer)
    jmp GDT64.code:halt_all

halt_all:
    cli
    hlt

.data

GDT64:                           // Global Descriptor Table (64-bit).
    .null: equ $ - GDT64         // The null descriptor.
    .int $0xFFFF                    // Limit (low).
    .int $0                         // Base (low).
    .byte $0                         // Base (middle)
    .byte $0                         // Access.
    .byte $1                         // Granularity.
    .byte $0                         // Base (high).
    .code: equ $ - GDT64         // The code descriptor.
    .int $0                         // Limit (low).
    .int $0                         // Base (low).
    .byte $0                         // Base (middle)
    .byte $0b10011010                 // Access (exec/read).
    .byte $0b10101111                 // Granularity, 64 bits flag, limit19:16.
    .byte $0                         // Base (high).
    .data: equ $ - GDT64         // The data descriptor.
    .int $0                         // Limit (low).
    .int $0                         // Base (low).
    .byte $0                         // Base (middle)
    .byte $0b10010010                 // Access (read/write).
    .byte $0b00000000                 // Granularity.
    .byte $0                         // Base (high).
    .pointer:                    // The GDT-pointer.
    .int $ - GDT64 - 1             // Limit.
    .long GDT64                     // Base.

