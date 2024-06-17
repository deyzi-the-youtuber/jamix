#ifndef __SYNC_H
#define __SYNC_H

#include <jamix/process.h>
#include <lib/common.h>

struct mutex
{
  bool status;
  struct task_struct * holder;
};

#endif
