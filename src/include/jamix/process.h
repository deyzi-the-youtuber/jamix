#ifndef __SCHED_H
#define __SCHED_H

#include <jamix/tty.h>
#include <stdint.h>
#include <lib/common.h>
#include <sys/types.h>

#define TASK_STATE_UNKNOWN          0
#define TASK_STATE_RUNNING		      1
#define TASK_STATE_INTERRUPTIBLE	  2
#define TASK_STATE_UNINTERRUPTIBLE	3
#define TASK_STATE_ZOMBIE		        4
#define TASK_STATE_STOPPED		      5
#define TASK_STATE_READY            6

struct task_context
{
  uint64_t rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi;
  uint64_t rip;
};

#define JAMIX_TASKING_MAGIC 'task'

struct task_struct
{
  uint64_t magic;
  int state;
  char * name;
  pid_t pid;
  pid_t ppid;   /* task parent pid. -1 if no parent */ 
  uint64_t * stack;
  uint8_t cpu_ticks;
  uint8_t time_quantum;
  struct tty_struct * ctty; /* task's controlling tty */
  struct task_context * ctx;
  struct task_struct * prev;
  struct task_struct * next;
};

struct task_struct * create_kernel_task(char * name, uintptr_t addr);
void task_release(struct task_struct * p);
struct task_struct * get_current_task(void);
bool sched_enabled(void);
void sched_set(bool status);

#endif
