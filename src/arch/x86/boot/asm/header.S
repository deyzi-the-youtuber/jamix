; multiboot 2 boot code

header_magic equ 0xe85250d6
bootloader_magic equ 0x36d76289
kernel_arch equ 0x0 ; i386 or x86_64
header_length equ multiboot_header_end - multiboot_header
checksum equ -(header_magic + kernel_arch + (header_length))

multiboot_header:
  align 8
  ; Header magic
  dd header_magic
  ; ISA: i386
  dd kernel_arch
  ; Header length.
  dd header_length
  ; Header checksum
  dd checksum
multiboot_fb_tag_start:
  dw 5
  dw 1
  dd multiboot_fb_tag_end - multiboot_fb_tag_start
  dd 1280
  dd 720
  dd 32
multiboot_fb_tag_end:
multiboot_header_tag_end:
  dw 0
  dw 0
  dd 0
multiboot_header_end:
