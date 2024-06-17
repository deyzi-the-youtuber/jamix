section .text
  global kernel_jump
  extern _start

kernel_jump:
  cli

  mov ax, 0
  mov ss, ax
  mov ds, ax 
  mov es, ax
  mov fs, ax
  mov gs, ax

  mov rsp, stack_top

  push qword 0
  popfq

  call _start

halt:
  hlt
  jmp halt

section .bss

stack_bottom:
  resb 0x400
stack_top:
