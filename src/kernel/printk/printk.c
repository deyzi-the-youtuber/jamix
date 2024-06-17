#include <stdarg.h>
#include <lib/common.h>

int printk(char * format, ...)
{
  va_list arg;
  va_start(arg, format);
  int ret = vprintf(format, arg);
  va_end(arg);
  return ret;
}
