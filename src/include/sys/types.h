#include <stdint.h>

#ifndef __TYPES_H
#define __TYPES_H

typedef uint16_t  dev_t;   // device numbers
typedef uint16_t  gid_t;   // group ids
typedef uint16_t  mode_t;  // some file attributes
typedef uint64_t  off_t;   // file sizes
typedef int64_t   pid_t;   // process ids
typedef uint64_t  size_t;  // unsigned long (uint64_t)
typedef int64_t   ssize_t; // signed long (int64_t)
typedef uint16_t  uid_t;   // user id

#endif
