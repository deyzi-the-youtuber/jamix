#include <jamix/process.h>
#include <jamix/printk.h>
#include <jamix/errno.h>
#include <jamix/tty.h>
#include <jamix/mem.h>
#include <lib/common.h>
#include <sys/types.h>
#include <stdint.h>

#pragma GCC diagnostic ignored "-Wmultichar"

static pid_t last_pid = 0;

static inline pid_t task_pid_alloc(void)
{
  return last_pid++;
}

static inline struct task_struct * task_struct_alloc(void)
{
  return (struct task_struct *)malloc(sizeof(struct task_struct));
}

struct task_struct * create_kernel_task(char * name, uintptr_t addr)
{
  /* allocate task structure */
  struct task_struct * p = task_struct_alloc();
  memset(p, 0, sizeof(struct task_struct));
  if(!p)
    return NULL;
  /* initialize basic entries */
  p->magic  = JAMIX_TASKING_MAGIC;
  p->state  = TASK_STATE_READY;
  p->pid    = task_pid_alloc();
  p->ppid   = -1;
  p->ctty   = tty_lookup(0);
  /* truncate name if needed */
  p->name   = name;
  /* set time quantum and other things */
  p->time_quantum = 3;
  p->cpu_ticks = p->time_quantum;
  /* now, onto the more lower level things.. */
  p->stack  = (uint64_t *)malloc(4096); /* allocate 4KiB of stack memory */
  if(!p->stack)
  {
    free(p);
    return NULL;
  }
  memset(p->stack, 0, 4096);
  /*
   *  To make the kernel task actually work, we need to push
   *  the return address (entry point) of the task into the
   *  stack. This makes it so that whenever we call a RET
   *  instruction with the task's stack, the CPU pops the 
   *  return address off of the stack, and into the
   *  instruction pointer, making kernel multitasking 
   *  possible.
   *
   *  Task Stack Layout
   *
   *  -----------------
   *  |   Task RIP    |
   *  -----------------
   *  |               |
   *  |               |
   *  |  Other things |
   *  |               |
   *  |               |
   *  |_______________|
   *  |               |
   *  |               |
   *  |               |
   *  |               |
   *  | Reserved Data |
   *  |               |
   *  |               |
   *  |               |
   *  |               |
   *  |_______________|
   *
   *
   */

#define STACK_PUSH(b) *--p->stack = b

  /* stack needs to be aligned */
  p->stack -= 16;
  STACK_PUSH(addr);    /* RIP */
  STACK_PUSH(0);                  /* RAX */
  STACK_PUSH(0);                  /* RBX */
  STACK_PUSH(0);                  /* RCX */
  STACK_PUSH(0);                  /* RDX */
  STACK_PUSH(0);                  /* RDI */
  STACK_PUSH(0);                  /* RSI */

  p->ctx->rsp = (uint64_t)(p->stack);
  p->ctx->rbp = p->ctx->rsp;
  return p;
}

void task_release(struct task_struct * p)
{
  free(p);
}
