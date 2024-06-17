#include <arch/ops.h>
#include <jamix/process.h>
#include <jamix/printk.h>
#include <jamix/errno.h>
#include <jamix/tty.h>
#include <jamix/mem.h>
#include <jamix/compiler.h>
#include <lib/list.h>
#include <lib/common.h>
#include <sys/types.h>
#include <stdint.h>

/* The kernel task does the scheduling for us */
struct task_struct * kernel_task;
struct task_struct * current_task;

bool resched_needed = false;
bool is_sched_enabled = false;

static void context_switch(void);

bool sched_enabled(void)
{
  return is_sched_enabled;
}

void sched_set(bool status)
{
  resched_needed = status;
}

static void scheduler_task(void)
{
  printk("Scheduler task created, time to expand...\n");
  is_sched_enabled = true;
  for(;;)
  {
    if(resched_needed == true)
    {
      context_switch();
    }
  }
}

static void context_switch(void)
{
  critical_enter();
  /* save task state */
  asm volatile("push %rax");
  asm volatile("push %rbx");
  asm volatile("push %rcx");
  asm volatile("push %rdx");
  asm volatile("push %rbp");
  asm volatile("push %rdi");
  asm volatile("push %rsi");
  asm volatile("mov %%rsp, %%rax" : "=a"(current_task->ctx->rsp));
  current_task = current_task->next;
  asm volatile("mov %%rax, %%rsp" :: "a"(current_task->ctx->rsp));
  asm volatile("pop %rsi");
  asm volatile("pop %rdi");
  asm volatile("pop %rbp");
  asm volatile("pop %rdx");
  asm volatile("pop %rcx");
  asm volatile("pop %rbx");
  asm volatile("pop %rax");
  critical_exit();
  asm volatile("ret");
  unreachable;
}

static void sched_exec(void)
{
  critical_enter();
  /* load task stack, and task registers */
  asm volatile("mov %%rax, %%rsp" :: "a"(current_task->ctx->rsp));
  asm volatile("pop %rsi");
  asm volatile("pop %rdi");
  asm volatile("pop %rbp");
  asm volatile("pop %rdx");
  asm volatile("pop %rcx");
  asm volatile("pop %rbx");
  asm volatile("pop %rax");
  critical_exit();
  asm volatile("ret");
  unreachable;
}

struct task_struct * get_current_task(void)
{
  return current_task;
}

void sched_init(void)
{
  kernel_task = create_kernel_task("scheduler", (uintptr_t)scheduler_task);
  if(!kernel_task)
    panic("failed to create scheduler task: %d\n", kernel_task);
  /* make it the only one */
  kernel_task->next = kernel_task;
  kernel_task->prev = kernel_task;
  current_task = kernel_task;
  sched_exec();
  panic("bug: shouldn't have returned!\n");
  unreachable;
}
