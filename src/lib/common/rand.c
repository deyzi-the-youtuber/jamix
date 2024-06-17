#include <stdint.h>
#include <lib/common.h>

uint64_t rand(void)
{
  static uint64_t x = 123456789;
  static uint64_t y = 362436069;
  static uint64_t z = 521288629;
  static uint64_t w = 88675123;

  uint64_t t;

  t = x ^ (x << 11);
  x = y; y = z; z = w;
  return w = w ^ (w >> 19) ^ t ^ (t >> 8);
}
