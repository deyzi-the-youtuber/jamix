#include <sys/types.h>
#include <lib/common.h>

void reverse(char * str, size_t sz)
{
  int start = 0;
  int end = sz - 1;
  while (start < end)
  {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    end--;
    start++;
  }
}
