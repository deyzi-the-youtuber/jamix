#include <arch/x86.h>
#include <arch/ops.h>
#include <jamix/printk.h>
#include <stdint.h>

void debug_irq(struct interrupt_frame * frame)
{
  printk("Received DEBUG exception at RIP: 0x%016x\n", frame->rip);
}

void breakpoint_irq(struct interrupt_frame * frame)
{
  printk("Received BREAKPOINT trap interrupt at RIP: 0x%016x. Halting kernel...\n", frame->rip);
  printk("RSP: 0x%016x, RBP: 0x%016x\n", frame->rsp, frame->rbp);
  printk("RDI: 0x%016x, RSI: 0x%016x\n", frame->rdi, frame->rsi);
  critical_enter();
  for(;;)
    asm volatile("hlt");
}

inline void exceptions_init(void)
{
  interrupt_install(debug_irq, 1);
  interrupt_install(breakpoint_irq, 3);
}
