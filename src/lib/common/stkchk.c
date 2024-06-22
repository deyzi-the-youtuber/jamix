#include <jamix/compiler.h>
#include <jamix/printk.h>
#include <stdint.h>

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void _noreturn_ __stack_chk_fail(void)
{
  panic("stack smashing detected");
  for(;;);
}
