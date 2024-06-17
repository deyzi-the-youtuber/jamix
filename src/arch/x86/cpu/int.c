#include <arch/x86.h>
#include <arch/ops.h>
#include <jamix/console.h>
#include <jamix/printk.h>
#include <stdint.h>
#include "pic.h"

static char * exceptions[32] = {
  "Divide Error",
  "Debug",
  "NMI Interrupt",
  "Breakpoint",
  "Overflow",
  "BOUND Range Exceeded",
  "Invalid Opcode",
  "Device Not Available",
  "Double Fault",
  "Coprocessor Segment Overrun",
  "Invalid TSS",
  "Segment Not Present",
  "Stack Segment Fault", 
  "General Protection Fault", 
  "Page Fault",
  "Reserved",
  "Floating-Point Error",
  "Alignment Check",
  "Machine Check",
  "SIMD Floating-Point Exception",
  "Virtualization Exception",
  "Control Protection Exception",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved"
};

void * int_handlers[256] = {NULL};

#define CALL_INT_HANDLER \
  void (*handler)(struct interrupt_frame * frame) = int_handlers[frame->intr]; \
  handler(frame);

void interrupt_install(void (*handler)(struct interrupt_frame *), int intr)
{
  if(!handler)
    return;
  if(int_handlers[intr])
  {
    printk("interrupt_install: already using interrupt 0x%02x\n", intr);
    return;
  }
  int_handlers[intr] = handler;
}

int exception_handler(struct interrupt_frame * frame)
{
  if(int_handlers[frame->intr])
  {
    CALL_INT_HANDLER;
    goto exit;
  }
  return -1;
exit:
  return 0;
}

void interrupt_handler(struct interrupt_frame * frame)
{
  if(frame->intr < 32)
  {
    if (exception_handler(frame) < 0)
    {
      if(is_console_enabled())
      {
        printk("RIP: 0x%016x EFLAGS: 0x%08x: RSP: 0x%016x\n", frame->rip, frame->rflags, frame->userrsp);
        printk("RAX: 0x%016x RBX: 0x%016x RCX: 0x%016x\n", frame->rax, frame->rbx, frame->rcx);
        printk("RDX: 0x%016x RDI: 0x%016x RSI: 0x%016x\n", frame->rdx, frame->rdi, frame->rsi);
        printk("RBP: 0x%016x R08: 0x%016x R09: 0x%016x\n", frame->rbp, frame->r8, frame->r9);
        printk("R10: 0x%016x R11: 0x%016x R12: 0x%016x\n", frame->r10, frame->r11, frame->r12);
        printk("R13: 0x%016x R14: 0x%016x R15: 0x%016x\n", frame->r13, frame->r14, frame->r15);
        printk("SS: 0x%04x CS: 0x%04x ERR: 0x%08x\n", frame->ss, frame->cs, frame->error_code);
        panic("Fatal exception: 0x%02x\n", frame->intr);
      }
      critical_enter();
      for(;;)
        asm volatile("hlt");
    }
  }
  else
  {
    if(int_handlers[frame->intr])
    {
      CALL_INT_HANDLER;
    }
    pic_eoi(frame->intr);
  }
}
