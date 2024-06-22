#include <arch/ops.h>
#include <arch/x86.h>
#include <jamix/process.h>
#include <jamix/printk.h>
#include <jamix/errno.h>
#include <jamix/tty.h>
#include <jamix/mem.h>
#include <jamix/debug.h>
#include <jamix/compiler.h>

#include <lib/common.h>
#include <sys/types.h>
#include <stdint.h>

#pragma GCC diagnostic ignored "-Wmultichar" 

struct task_struct * kernel_task;
struct task_struct * current;

static bool is_sched_enabled = false;
int total_tasks = 0;

extern void context_switch(uint64_t old_rsp, uint64_t next_esp);
extern void sched_tasking_enter(uint64_t rsp);

bool sched_enabled(void)
{
  return is_sched_enabled;
}

struct task_struct * get_current_task(void)
{
  return current;
}

void schedule(void)
{
  if(current->ticks_left != 0)
  {
    current->ticks_left--;
    return;
  }
  critical_enter();
  struct task_struct * old = NULL;
  /* reset cpu ticks left until reschedule */
  current->ticks_left = current->time_quantum;
  /* change out old task for the next one */
  while(current->state != TASK_STATE_READY)
    current = current->next;

  old = current;
  /* change task states */
  old->state      = TASK_STATE_READY;
  current->state  = TASK_STATE_RUNNING;
  /* set tss stack */
  tss_set_stack(current->rsp);
  debug("[scheduler] SWITCH! old_rsp=0x%016x -> next_rsp=0x%016x\n", old->rsp, current->rsp);
  context_switch(old->rsp, current->rsp);
}

void sched_queue(struct task_struct * p)
{
  critical_enter();
  if(total_tasks > 1)
  {
    p->next = current->next;
    p->next->prev = p;
    p->prev = current;
    current->next = p;
  }
  else
  {
    p->next = p;
    p->prev = p;
    current = p;
  }
  total_tasks++;
  critical_exit();
}

struct task_struct * sched_proc_search(pid_t pid)
{
  struct task_struct * p = kernel_task;
  while(p != kernel_task)
  {
    if(p->pid == pid)
      return p;
    p = p->next;
  }
  return NULL;
}

int kill(pid_t pid)
{
  if(pid == 0)
  {
    printk("Kernel task cannot be killed\n");
    return -EPERM;
  }
  struct task_struct * p = sched_proc_search(pid);
  if(!p)
    return -ESRCH;
  critical_enter();
  task_release(p);
  p->prev->next = p->next;
  p->next->prev = p->prev;
  p->state = TASK_STATE_UNKNOWN;
  total_tasks--;
  critical_exit();
  return 0;
}

static void kernel_idle(void)
{
  is_sched_enabled = true;
  printk("Fortnite Sigma Rizzler OSDever. This has to be the most Gen-Z Operating System kernel ever\n");
  while(1);
}

void _noreturn_ sched_init(void)
{
  printk("Scheduler: Creating kernel task...\n");
  memset(current, 0, sizeof(struct task_struct));
  kernel_task = create_kernel_task("kernel", (uintptr_t)kernel_idle);
  if(!kernel_task)
    panic("Failed to start kernel late init\n");
  sched_queue(kernel_task);
  tss_set_stack(current->rsp);
  sched_tasking_enter(current->rsp);
  panic("You should not be here. Report this bug to the Jamix GitHub repository");
  unreachable;
}
