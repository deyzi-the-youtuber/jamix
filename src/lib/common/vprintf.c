#include <stdarg.h>
#include <lib/common.h>
#include <jamix/console.h>

int vprintf(char * format, va_list arg)
{
  char buf[256];
  int ret = vsprintf(buf, format, arg);
  console_puts(buf);
  return ret;
}
