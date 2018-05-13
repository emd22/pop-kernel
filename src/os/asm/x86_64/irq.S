.macro pushaq
    push %rax
    push %rcx
    push %rdx
    push %rbx
    push %rbp
    push %rsi
    push %rdi
.endm # pushaq

.macro popaq
    pop %rdi    
    pop %rsi    
    pop %rbp    
    pop %rbx
    pop %rdx    
    pop %rcx
    pop %rax
.endm # popaq

.section .text
.align 4
  
.extern irq_handler

# irq stuff
.global _irq0
.global _irq1
.global _irq2
.global _irq3
.global _irq4
.global _irq5
.global _irq6
.global _irq7
.global _irq8
.global _irq9
.global _irq10
.global _irq11
.global _irq12
.global _irq13
.global _irq14
.global _irq15

# 32: IRQ0
_irq0:
  cli
  push $0
  push $32
  jmp irq_common_stub

# 33: IRQ1
_irq1:
  cli
  push $0
  push $33
  jmp irq_common_stub
  
# 34: IRQ2
_irq2:
  cli
  push $0
  push $34
  jmp irq_common_stub
  
# 35: IRQ3
_irq3:
  cli
  push $0
  push $35
  jmp irq_common_stub
  
# 36: IRQ4
_irq4:
  cli
  push $0
  push $36
  jmp irq_common_stub
  
# 37: IRQ5
_irq5:
  cli
  push $0
  push $37
  jmp irq_common_stub
  
# 38: IRQ6
_irq6:
  cli
  push $0
  push $38
  jmp irq_common_stub
  
# 39: IRQ7
_irq7:
  cli
  push $0
  push $39
  jmp irq_common_stub
  
# 40: IRQ8
_irq8:
  cli
  push $0
  push $40
  jmp irq_common_stub
  
# 41: IRQ9
_irq9:
  cli
  push $0
  push $41
  jmp irq_common_stub
  
# 42: IRQ10
_irq10:
  cli
  push $0
  push $42
  jmp irq_common_stub
  
# 43: IRQ11
_irq11:
  cli
  push $0
  push $43
  jmp irq_common_stub
  
# 44: IRQ12
_irq12:
  cli
  push $0
  push $44
  jmp irq_common_stub
  
# 45: IRQ13
_irq13:
  cli
  push $0
  push $45
  jmp irq_common_stub

# 46: IRQ14
_irq14:
  cli
  push $0
  push $46
  jmp irq_common_stub
  
# 47: IRQ15
_irq15:
  cli
  push $0
  push $47
  jmp irq_common_stub

.type irq_handler, @function

irq_common_stub:
  /* Save all registers */
  pushaq

  /* Save segment registers */
  push %dx
  push %ax
  push %bx
  push %cx
  mov $0x10, %ax
  mov %ax, %dx
  mov %ax, %bx
  mov %ax, %cx
  mov %ax, %gs

  /* Call interrupt handler */
  push %rsp
  call irq_handler
  add $4, %rsp

  /* Restore segment registers */
  pop %dx
  pop %ax
  pop %bx
  pop %cx

  /* Restore all registers */
  popaq
  /* Cleanup error code and IRQ number */
  add $8, %rsp
  /* pop CS, EIP, EFLAGS, SS and ESP */
  iret
