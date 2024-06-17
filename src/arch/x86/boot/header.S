[bits 32]

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
  dd 800
  dd 600
  dd 32
multiboot_fb_tag_end:
multiboot_header_tag_end:
  dw 0
  dw 0
  dd 0
multiboot_header_end:

section .text
  global grub_load
  global boot_pud
  extern kernel_jump

grub_load:
  ; long mode setup
  cmp eax, bootloader_magic
  jne .err

  mov esi, ebx
  mov edi, eax

  call cpuid_detect
  call long_mode_detect
  call page_table_setup
  call paging_enable

  lgdt [gdt_64.ptr]
  jmp gdt_64.code_seg:kernel_jump

.err:
  jmp $

cpuid_detect:
  pushfd
  pop eax
  mov ecx, eax
  xor eax, 1 << 21
  push eax
  popfd
  pushfd
  pop eax
  push ecx
  popfd
  cmp eax, ecx
  je grub_load.err
  ret

long_mode_detect:
  mov eax, 0x80000000
  cpuid 
  cmp eax, 0x80000001
  jb grub_load.err
  mov eax, 0x80000001
  cpuid
  test edx, 1 << 29
  jz grub_load.err
  ret

page_table_setup:
  ; UEFI BIOS might turn on paging. disable it.
  mov eax, cr0
  mov ebx, 0x80000000
  not ebx       ; flip bits
  and eax, ebx  ; clear paging bit
  mov cr0, eax  ; basically acts like this: CR0 &= ~0x80000000

  mov eax, boot_pud
  or eax, 1 << 0 | 1 << 1     ; read-write, and present
  mov [boot_pgd], eax

  xor eax, eax
  or eax, (1 << 0 | 1 << 1 | 1 << 7) ; read-write, present, page-size
  mov [boot_pud], eax

  xor eax, eax
  mov [boot_pgd + 4], eax
  mov [boot_pud + 4], eax

  ret

paging_enable:
	; pass page table location to cpu
	mov eax, boot_pgd
	mov cr3, eax

	; enable PAE
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	; enable long mode
	mov ecx, 0xc0000080
	rdmsr
	or eax, 1 << 8 ; set EFER.LME bit
	wrmsr

	; enable paging
	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax

	ret

section .bss

align 4096

boot_pgd:
  resb 4096
boot_pud:
  resb 4096

section .rodata
gdt_64:
  dq 0 
.code_seg: equ $ - gdt_64
  dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
.ptr:
  dw $ - gdt_64 - 1 
  dq gdt_64
