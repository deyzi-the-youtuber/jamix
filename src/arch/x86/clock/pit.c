#include <arch/x86.h>
#include <jamix/errno.h>
#include <jamix/math.h>
#include <jamix/mem.h>
#include <jamix/process.h>
#include <sys/io.h>
#include <stdint.h>

#define PIT_IRQ 32

volatile uint64_t jiffies = 0;
uint16_t freq;
int num_cbs = 0;

struct callback
{
  void (*func)(void);
  uint64_t ms;
  uint64_t expiration;
};

struct callback * callbacks[8] = {0};

int create_callback(void (*cb_func)(void), uint64_t ms)
{
  struct callback * cb = (struct callback *)malloc(sizeof(struct callback));
  if(!cb)
    return -ENOMEM;
  cb->func = cb_func;
  cb->ms = ms;
  callbacks[num_cbs++] = cb;
  return 0;
}

static inline void check_callbacks(void)
{
  for(int i = 0; callbacks[i]; i++)
  {
    if(callbacks[i]->expiration)
    {
      callbacks[i]->expiration--;
    }
    else
    {
      callbacks[i]->expiration = callbacks[i]->ms;
      callbacks[i]->func();
    }
  }
}

static void pit_configure_hz(uint16_t hz)
{ 
  freq = hz;
  int divisor = 1193180 / hz;  
  outb(0x43, 0x36); 
  outb(0x40, divisor & 0xff);
  outb(0x40, divisor >> 8);
}

void timer_irq(struct interrupt_frame * frame)
{
  jiffies++;
  if(sched_enabled())
  {
    schedule();
  }
  check_callbacks();
}

void usleep(uint64_t ms)
{
  uint64_t end_jiffies = jiffies + ms;
  while (jiffies < end_jiffies);
}

void timer_init(void)
{
  pit_configure_hz(100);
  interrupt_install(timer_irq, PIT_IRQ);
}
