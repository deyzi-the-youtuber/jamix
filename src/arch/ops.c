#include <arch/x86.h>

void critical_enter(void)
{
  arch_disable_ints();
}

void critical_exit(void)
{
  arch_enable_ints();
}
