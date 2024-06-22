#include <stdarg.h>
#include <lib/common.h>

int snprintf(char * str, size_t len, char * fmt, ...)
{
  int rc;
  va_list arg;
  va_start(arg, fmt);
  rc = vsnprintf(str, len, fmt, arg);
  va_end(arg);
  return rc;
}
