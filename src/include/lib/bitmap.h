#ifndef __BITMAP_H
#define __BITMAP_H

#include <jamix/bitops.h>

#define CREATE_BITMAP(name, bits) \
  uint64_t name[BITS_TO_U64(bits)]

#endif
