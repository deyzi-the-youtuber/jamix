section .text
  extern interrupt_handler
  %include "arch/x86/cpu/asm/common.inc"

early_interrupt_handler:
  pushaq                ; push all registers

  mov ax, ds
  push rax              ; save ds

  mov ax, 0x10          ; load kernel data segment
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  mov rdi, rsp
  cld
  call interrupt_handler

  pop rax                ; restore kernel data segment
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  popaq               ; restore all registers
  sti
  iretq

%macro DEFINE_INT_HANDLER 2
global interrupt_handler_%1
interrupt_handler_%1:
  cli
%if %2 == 1
  push byte 0
%endif
  push %1
  jmp early_interrupt_handler
%endmacro

DEFINE_INT_HANDLER 0, 1
DEFINE_INT_HANDLER 1, 1
DEFINE_INT_HANDLER 2, 1
DEFINE_INT_HANDLER 3, 1
DEFINE_INT_HANDLER 4, 1
DEFINE_INT_HANDLER 5, 1
DEFINE_INT_HANDLER 6, 1
DEFINE_INT_HANDLER 7, 1
DEFINE_INT_HANDLER 8, 0
DEFINE_INT_HANDLER 9, 1
DEFINE_INT_HANDLER 10, 0
DEFINE_INT_HANDLER 11, 0
DEFINE_INT_HANDLER 12, 0
DEFINE_INT_HANDLER 13, 0
DEFINE_INT_HANDLER 14, 0
DEFINE_INT_HANDLER 15, 1
DEFINE_INT_HANDLER 16, 1
DEFINE_INT_HANDLER 17, 0
DEFINE_INT_HANDLER 18, 1
DEFINE_INT_HANDLER 19, 1
DEFINE_INT_HANDLER 20, 1
DEFINE_INT_HANDLER 21, 0
DEFINE_INT_HANDLER 22, 1
DEFINE_INT_HANDLER 23, 1
DEFINE_INT_HANDLER 24, 1
DEFINE_INT_HANDLER 25, 1
DEFINE_INT_HANDLER 26, 1
DEFINE_INT_HANDLER 27, 1
DEFINE_INT_HANDLER 28, 1
DEFINE_INT_HANDLER 29, 0
DEFINE_INT_HANDLER 30, 0
DEFINE_INT_HANDLER 31, 1
DEFINE_INT_HANDLER 32, 1
DEFINE_INT_HANDLER 33, 1
DEFINE_INT_HANDLER 34, 1
DEFINE_INT_HANDLER 35, 1
DEFINE_INT_HANDLER 36, 1
DEFINE_INT_HANDLER 37, 1
DEFINE_INT_HANDLER 38, 1
DEFINE_INT_HANDLER 39, 1
DEFINE_INT_HANDLER 40, 1
DEFINE_INT_HANDLER 41, 1
DEFINE_INT_HANDLER 42, 1
