#include <lib/common.h>
#include <stdarg.h>

int vsprintf(char * str, char * format, va_list arg)
{
  return vsnprintf(str, __INT_MAX__, format, arg);
}
