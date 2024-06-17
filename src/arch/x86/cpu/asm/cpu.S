section .text
  global load_gdt
  global load_idt

reload_segm:
   mov ax, 0x10 
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov ss, ax
   ret

load_gdt:
  lgdt [rdi]
  push 0x08
  lea rax, [rel reload_segm]
  push rax
  retfq

load_idt:
  lidt [rdi]
  sti
  ret
