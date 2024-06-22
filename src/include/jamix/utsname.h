#ifndef __UTSNAME_H
#define __UTSNAME_H

#define MAX_LENGTH 65

#define UTS_SYSNAME "Jamix"
#define UTS_NODENAME "jamix"
#define UTS_RELEASE "v1.00"
#define UTS_VERSION (__DATE__ " " __TIME__)
#ifdef __i386__
#define UTS_MACHINE "i386"
#else
#define UTS_MACHINE "x86_64"
#endif

struct utsname
{
  char sysname[MAX_LENGTH];
  char nodename[MAX_LENGTH];
  char release[MAX_LENGTH];
  char version[MAX_LENGTH];
  char machine[MAX_LENGTH];
};

#endif
