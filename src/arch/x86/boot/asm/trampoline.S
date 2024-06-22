[bits 32]

section .text
  global grub_load
  global boot_pud
  extern kernel_jump

grub_load:
  ; long mode setup
  cmp eax, 0x36d76289 ; bootloader magic
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
