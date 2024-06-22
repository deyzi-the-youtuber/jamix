#include <stdarg.h>
#include <lib/common.h>

int sprintf(char * str, char * fmt, ...)
{
  int rc;
  va_list arg;
  va_start(arg, fmt);
  rc = vsprintf(str, fmt, arg);
  va_end(arg);
  return rc;
}
