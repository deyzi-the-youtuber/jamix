#include <jamix/printk.h>
#include <jamix/compiler.h>
#include <lib/common.h>
#include <arch/ops.h>
#include <stdarg.h>

static void die()
{
  critical_enter();
  for(;;);
}

void panic(char * format, ...)
{
  va_list ap;
  va_start(ap, format);
  vprintf(strcat("Kernel Panic - ", format), ap);
  va_end(ap);
  die();
  unreachable;
}

inline void panic_crit(void)
{
  die();
  unreachable;
}
