#ifndef __FS_H
#define __FS_H

#include <sys/types.h>

struct file
{
  mode_t f_mode;
  dev_t rdev;       /* needed for /dev/tty devices */
  off_t f_pos;
  uint16_t f_flags;
  uint16_t f_count;
  struct inode * f_inode;
  struct file_operations * f_ops;
};

#endif
