#ifndef __ARCH_X86_H
#define __ARCH_X86_H

#include <stdint.h>
#include <jamix/compiler.h>

/*
  * Global Descriptor Table
*/

struct segm_descriptor
{
  uint16_t limit;
  uint16_t base_low;
  uint8_t  base_mid;
  uint8_t  access;
  uint8_t  flags;
  uint8_t  base_high;
}_packed_;

struct gdtr
{
  uint16_t limit;
  uintptr_t addr;
}_packed_;

/*
  * Interrupt Descriptor Table
*/

struct interrupt_vector
{
  uint16_t  offset_low;
  uint16_t  selector;
  uint8_t   ist;
  uint8_t   flags;
  uint16_t  offset_mid;
  uint32_t  offset_high;
  uint32_t  reserved;
}_packed_;

struct idtr
{
  uint16_t limit;
  uint64_t addr;
}_packed_;

/*
  * Interrupt Handlers
*/

extern void interrupt_handler_0();
extern void interrupt_handler_1();
extern void interrupt_handler_2();
extern void interrupt_handler_3();
extern void interrupt_handler_4();
extern void interrupt_handler_5();
extern void interrupt_handler_6();
extern void interrupt_handler_7();
extern void interrupt_handler_8();
extern void interrupt_handler_9();
extern void interrupt_handler_10();
extern void interrupt_handler_11();
extern void interrupt_handler_12();
extern void interrupt_handler_13();
extern void interrupt_handler_14();
extern void interrupt_handler_15();
extern void interrupt_handler_16();
extern void interrupt_handler_17();
extern void interrupt_handler_18();
extern void interrupt_handler_19();
extern void interrupt_handler_20();
extern void interrupt_handler_21();
extern void interrupt_handler_22();
extern void interrupt_handler_23();
extern void interrupt_handler_24();
extern void interrupt_handler_25();
extern void interrupt_handler_26();
extern void interrupt_handler_27();
extern void interrupt_handler_28();
extern void interrupt_handler_29();
extern void interrupt_handler_30();
extern void interrupt_handler_31();
extern void interrupt_handler_32();
extern void interrupt_handler_33();
extern void interrupt_handler_34();
extern void interrupt_handler_35();
extern void interrupt_handler_36();
extern void interrupt_handler_37();
extern void interrupt_handler_38();
extern void interrupt_handler_39();
extern void interrupt_handler_40();
extern void interrupt_handler_41();
extern void interrupt_handler_42();

static uint64_t interrupt_handlers[43] = {
  (uint64_t)interrupt_handler_0,
  (uint64_t)interrupt_handler_1,
  (uint64_t)interrupt_handler_2,
  (uint64_t)interrupt_handler_3,
  (uint64_t)interrupt_handler_4,
  (uint64_t)interrupt_handler_5,
  (uint64_t)interrupt_handler_6,
  (uint64_t)interrupt_handler_7,
  (uint64_t)interrupt_handler_8,
  (uint64_t)interrupt_handler_9,
  (uint64_t)interrupt_handler_10,
  (uint64_t)interrupt_handler_11,
  (uint64_t)interrupt_handler_12,
  (uint64_t)interrupt_handler_13,
  (uint64_t)interrupt_handler_14,
  (uint64_t)interrupt_handler_15,
  (uint64_t)interrupt_handler_16,
  (uint64_t)interrupt_handler_17,
  (uint64_t)interrupt_handler_18,
  (uint64_t)interrupt_handler_19,
  (uint64_t)interrupt_handler_20,
  (uint64_t)interrupt_handler_21,
  (uint64_t)interrupt_handler_22,
  (uint64_t)interrupt_handler_23,
  (uint64_t)interrupt_handler_24,
  (uint64_t)interrupt_handler_25,
  (uint64_t)interrupt_handler_26,
  (uint64_t)interrupt_handler_27,
  (uint64_t)interrupt_handler_28,
  (uint64_t)interrupt_handler_29,
  (uint64_t)interrupt_handler_30,
  (uint64_t)interrupt_handler_31,
  (uint64_t)interrupt_handler_32,
  (uint64_t)interrupt_handler_33,
  (uint64_t)interrupt_handler_34,
  (uint64_t)interrupt_handler_35,
  (uint64_t)interrupt_handler_36,
  (uint64_t)interrupt_handler_37,
  (uint64_t)interrupt_handler_38,
  (uint64_t)interrupt_handler_39,
  (uint64_t)interrupt_handler_40,
  (uint64_t)interrupt_handler_41,
  (uint64_t)interrupt_handler_42
};

struct interrupt_frame
{
  uint64_t ds;
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
  uint64_t rbp, rdi, rsi, rbx, rdx, rcx, rax;
  uint64_t intr, error_code, rip, cs, eflags, rsp, ss;
};

/* task state segment */

struct tss_descriptor
{
  uint32_t reserved0;
  uint64_t rsp0;
  uint64_t rsp1;
  uint64_t rsp2;
  uint32_t reserved1;
  uint32_t reserved2;
  uint64_t ist1;
  uint64_t ist2;
  uint64_t ist3;
  uint64_t ist4;
  uint64_t ist5;
  uint64_t ist6;
  uint64_t ist7;
  uint32_t reserved3;
  uint32_t reserved4;
  uint16_t iopb;
}_packed_;

void exceptions_init(void);

void arch_enable_ints(void);
void arch_disable_ints(void);
void interrupt_install(void (*handler)(struct interrupt_frame *), int intr);
void tss_set_stack(uint64_t rsp);

#endif
