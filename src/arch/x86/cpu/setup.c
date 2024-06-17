#include <stdint.h>
#include <arch/x86.h>
#include <lib/common.h>
#include "pic.h"

extern uint64_t boot_pud[512];

/*
  * Global Descriptor Table
*/

#define GDT_DESC_ACCESSED     BIT(0)
#define GDT_DESC_RW           BIT(1)
#define GDT_DESC_CONFORMING   BIT(2)
#define GDT_DESC_EXEC         BIT(3)
#define GDT_DESC_SYS          BIT(4)
#define GDT_DPL_USER          2
#define GDT_DPL_KERNEL        0
#define GDT_DESC_PRESENT      BIT(7)

#define GDT_FLAG_64BIT        BIT(1)
#define GDT_FLAG_SEL32        BIT(2)
#define GDT_FLAG_GRAN         BIT(3)

#define GDT_SEGM_MAX 8

static struct segm_descriptor gdt[GDT_SEGM_MAX];

#define GDT_SEGM_KERNEL_CODE (GDT_DESC_RW | GDT_DESC_EXEC | GDT_DESC_SYS | GDT_DPL_KERNEL | GDT_DESC_PRESENT)
#define GDT_SEGM_KERNEL_DATA (GDT_DESC_RW | GDT_DESC_SYS | GDT_DPL_KERNEL | GDT_DESC_PRESENT)

#define GDT_SEGM_USER_CODE   (GDT_DESC_RW | GDT_DESC_EXEC | GDT_DESC_SYS | GDT_DPL_USER | GDT_DESC_PRESENT)
#define GDT_SEGM_USER_DATA   (GDT_SEGM_USER_CODE ^ GDT_DESC_EXEC)

static struct gdtr gdt_reg = {
  .limit = sizeof(gdt) - 1,
  .addr = (uint64_t)&gdt
};

extern void timer_init(void);
extern void load_gdt(uint64_t gdtr);

void set_global_descriptor(int descriptor, uint8_t access, uint8_t flags)
{
  gdt[descriptor].access = access;
  gdt[descriptor].flags = flags << 4;
}

void gdt_setup(void)
{
  // (0x00)
  set_global_descriptor(0, 0, 0);
  // (0x08)
  set_global_descriptor(1, GDT_SEGM_KERNEL_CODE, GDT_FLAG_64BIT); // kernel code segm 64
  // (0x10)
  set_global_descriptor(2, GDT_SEGM_KERNEL_DATA, 0); // kernel data segm 64 
  // (0x18)
  set_global_descriptor(3, GDT_SEGM_USER_CODE, 0); // user code segm 64
  // (0x20)
  set_global_descriptor(4, GDT_SEGM_USER_DATA, GDT_FLAG_64BIT); // user data segm 64
  load_gdt((uint64_t)&gdt_reg);
}

/*
  * Interrupt Descriptor Table
*/

#define IDT_INTERRUPT_GATE  0x8e
#define IDT_TRAP_GATE       0x8f

static struct interrupt_vector idt[256];

static struct idtr idt_reg = {
  .limit = sizeof(idt) - 1,
  .addr = (uint64_t)&idt
};

void set_interrupt_vector(int vector, uint64_t offset, uint8_t ist, uint8_t flags)
{
  idt[vector].offset_low = (uint16_t)offset & 0xffff;
  idt[vector].offset_mid = (uint16_t)(offset >> 16) & 0xffff;
  idt[vector].offset_high = (uint32_t)(offset >> 32) & 0xffffffff;
  idt[vector].selector = 0x08;
  idt[vector].ist = ist & 0x0f;
  idt[vector].flags = flags | 0x60; // fills in the DPL part of the structure
  idt[vector].reserved = 0;
}

extern void load_idt(uint64_t idtr);

void idt_setup(void)
{
  pic_remap_vectors();
  for(int i = 0; i < sizeof(interrupt_handlers) / sizeof(interrupt_handlers[0]); i++)
  {
    uint8_t flags = 0;
    if(i > 32)
      flags = IDT_TRAP_GATE;
    else
      flags = IDT_INTERRUPT_GATE;
    set_interrupt_vector(i, interrupt_handlers[i], 0, flags); // we dont use the IST for now
  }
  load_idt((uint64_t)&idt_reg);
}

void arch_early_init(void)
{
  /* Map 512 GB of mem */
  for (uint64_t i = 1; i < 512; i++)
  {
    boot_pud[i] = (i * 0x40000000) | (BIT(0) | BIT(1) | BIT(7));
  }
  gdt_setup();
  idt_setup();
  exceptions_init();
  timer_init();
}
