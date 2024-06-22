#ifndef __FS_H
#define __FS_H

#include <jamix/limits.h>
#include <jamix/dirent.h>
#include <sys/types.h>

struct file;

struct file_operations
{
	int (*read) (struct file *, void *, size_t);
	int (*write) (struct file *, void *, size_t);
	int (*readdir) (struct file *, void *, struct dirent *, int);
	int (*ioctl) (struct file *, void *, uint16_t, uint32_t);
	int (*open) (struct file *, void *);
	void (*close) (struct file *, void *);
};

struct file
{
  mode_t mode;
  off_t pos;
  uint16_t flags;
  uint16_t count;
  const char name[NAME_MAX + 1];
  const char path[NAME_MAX + 1];
  struct file_operations * ops;
};

#endif
