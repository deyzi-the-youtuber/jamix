#ifndef __DIRENT_H
#define __DIRENT_H

#include <jamix/limits.h>
#include <sys/types.h>
#include <stdint.h>

/* directory entry */
struct dirent
{
	uintptr_t	d_ino;
	off_t	d_off;
	uint16_t d_reclen;
	char d_name[NAME_MAX + 1];
};

#endif
