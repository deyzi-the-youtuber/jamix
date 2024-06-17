#ifndef __DEVICE_H
#define __DEVICE_H

#include <sys/types.h>
#include <lib/common.h>
#include <jamix/tty.h>

enum device_type
{
  DEVICE_TYPE_INVALID = 0,
  DEVICE_TYPE_CHAR,
  DEVICE_TYPE_BLOCK
};

enum device_class
{
  DEVICE_CLASS_UNKNOWN = 0,
  DEVICE_CLASS_CONSOLE
};

struct tty_struct;

/* basic device structure */
struct device
{
  const char * init_name;               /* initial name of device */
  int device_type;                      /* type of device */
  int device_class;                     /* class of device */
  bool removable;                       /* can it be removed? */
  struct device * parent;               /* parent of the device (if it has one) */
  int (*tty_output_intr)(struct tty_struct *, size_t);
};

void device_init(void);

#endif
