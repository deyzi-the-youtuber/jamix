#include <jamix/process.h>
#include <jamix/math.h>
#include <jamix/printk.h>
#include <jamix/errno.h>
#include <jamix/tty.h>
#include <jamix/mem.h>
#include <jamix/debug.h>
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

struct task_struct * create_kernel_task(char name[], uintptr_t addr)
{
  /* allocate task structure */
  struct task_struct * p = task_struct_alloc(); 
  if(!p)
    return NULL;
  memset(p, 0, sizeof(struct task_struct));
  /* initialize basic entries */
  p->magic      = JAMIX_TASKING_MAGIC;
  p->state      = TASK_STATE_READY;
  p->pid        = task_pid_alloc();
  p->ppid       = -1;
  p->kernel     = true;
  p->ctty       = tty_lookup(0);
  p->name       = name;
  p->cwd        = NULL;
  p->priority   = TASK_PRIORITY_HIGH; /* kernel task priority is automatically set to the highest. this is the kernel after all.. */
  /* set time quantum and other things */
  switch(p->priority)
  {
    case TASK_PRIORITY_LOW:
    {
      p->time_quantum = 1;
      break;
    }
    case TASK_PRIORITY_NORMAL:
    {
      p->time_quantum = 3;
      break;
    }
    case TASK_PRIORITY_HIGH:
    {
      p->time_quantum = 5;
      break;
    }
    default:
    {
      break;    
    }
  }
  p->ticks_left     = p->time_quantum;
  /* now, onto the more lower level things.. */
  uintptr_t * stack;
  p->rsp = (uintptr_t)malloc(TASK_STACK_SIZE);
  if(!((void *)p->rsp))
  {
    free(p);
    return NULL;
  }
  p->stack_top = (uintptr_t)p->rsp + TASK_STACK_SIZE;
  stack = (uintptr_t *)p->stack_top;

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

#define STACK_PUSH(n) \
  (*(--stack) = (n))
  
  STACK_PUSH(0x10);
  STACK_PUSH(p->stack_top);
  STACK_PUSH(0x202);
  STACK_PUSH(0x08);
  STACK_PUSH(addr);                         /* RIP */
  STACK_PUSH(0);                            /* RAX */
  STACK_PUSH(0);                            /* RBX */
  STACK_PUSH(0);                            /* RCX */
  STACK_PUSH(0);                            /* RDX */
  STACK_PUSH(p->stack_top);                 /* RBP */
  STACK_PUSH(0);                            /* RDI */
  STACK_PUSH(0);                            /* RSI */
  STACK_PUSH(0);                            /* R08  */
  STACK_PUSH(0);                            /* R09  */
  STACK_PUSH(0);                            /* R10  */
  STACK_PUSH(0);                            /* R11  */
  STACK_PUSH(0);                            /* R12  */
  STACK_PUSH(0);                            /* R13  */
  STACK_PUSH(0);                            /* R14  */
  STACK_PUSH(0);                            /* R15  */
  p->rsp = (uintptr_t)stack;
  debug("[create_task] Created task \"%s\" with pid %d. Info: RSP=0x%016x, RIP=0x%016x\n", p->name, p->pid, p->rsp, addr);
  return p;
}

#undef STACK_PUSH

void task_release(struct task_struct * p)
{
  if(p)
  {
    if((void *)p->stack_top)
    {
      free((void *)p->stack_top);
    }
    free(p);
  }
}
