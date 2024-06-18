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

/*The kernel task does the scheduling for us */
struct task_struct * kernel_task;
struct task_struct * current_task;

bool is_sched_enabled = false;

void task_thing(void);
void sched_add_queue(struct task_struct * p);
extern void context_switch(struct task_struct * c, uint64_t next_esp);
extern void sched_task_init(uint64_t esp);

bool sched_enabled(void)
{
  return is_sched_enabled;
}

void sched_enable_set(bool status)
{
  is_sched_enabled = status;
}

void schedule(void)
{
  if(current_task->cpu_ticks)
  {
    current_task->cpu_ticks--;
    return;
  }
  struct task_struct * new_task = current_task->next;
  struct task_struct * old_task = current_task;
  while(new_task->state != TASK_STATE_READY)
  {
    new_task = new_task->next;
  }
  debug("Task name: %s, Task RSP: 0x%016x\n", new_task->name, new_task->ctx->rsp);
  current_task = new_task;
  context_switch(old_task, new_task->ctx->rsp);
}

void task_thing(void)
{
  is_sched_enabled = true;
  printk("hi");
  for(;;);
}

void sched_add_queue(struct task_struct * p)
{
  critical_enter();
  p->prev = current_task;
  p->next = current_task->next;
  p->next->prev = p;
  current_task->next = p;
  critical_exit();
}

struct task_struct * get_current_task(void)
{
  return current_task;
}

void sched_init(void)
{
  kernel_task = create_kernel_task("task", (uintptr_t)task_thing);
  if(!kernel_task)
    panic("failed to create task\n");
  /* make it the only one */
  kernel_task->next = kernel_task;
  kernel_task->prev = kernel_task;
  current_task = kernel_task;
  sched_task_init(current_task->ctx->rsp);
  unreachable;
}
