#ifndef __PROCESS_H
#define __PROCESS_H

#include <jamix/tty.h>
#include <stdint.h>
#include <lib/common.h>
#include <sys/types.h>

enum task_state
{
  TASK_STATE_UNKNOWN = 0,
  TASK_STATE_RUNNING,
  TASK_STATE_INTERRUPTIBLE,
  TASK_STATE_UNINTERRUPTIBLE,
  TASK_STATE_ZOMBIE,
  TASK_STATE_STOPPED,
  TASK_STATE_READY
};

enum task_priority
{
  TASK_PRIORITY_LOW,
  TASK_PRIORITY_NORMAL,
  TASK_PRIORITY_HIGH
};

#define JAMIX_TASKING_MAGIC 'task'
#define TASK_STACK_SIZE 4096
#define NPROC 32

struct task_struct
{
  uint64_t magic;             /* should always be 'task' in hex */
  int state;
  int priority;
  char * name;
  char * cwd;                 /* current working directory of the task */
  pid_t pid;
  pid_t ppid;                 /* task parent pid. -1 if no parent */ 
  bool kernel;                /* 1 = kernel mode, 0 = user-mode */
  uintptr_t stack_top;         /* top of the task's stack */
  uintptr_t rsp;
  uint8_t ticks_left;
  uint8_t time_quantum;
  struct tty_struct * ctty;   /* task's controlling tty */
  struct task_struct * next;  /* next task */
  struct task_struct * prev;  /* previous task */
};

struct task_struct * create_kernel_task(char * name, uintptr_t addr);
void task_release(struct task_struct * p);

void sched_init(void);
void schedule(void);
bool sched_enabled(void);

struct task_struct * get_current_task(void);

#endif
