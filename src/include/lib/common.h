#ifndef __COMMON_H
#define __COMMON_H

#include <sys/types.h>
#include <stdarg.h>

#define BIT(bit) (1 << bit)

#define ALIGN(align, num) (num + align - 1) & ~(align - 1)

typedef enum
{
  false = 0,
  true = 1
}bool;

#ifndef NULL
#define NULL ((void *)0)
#endif

#define PTR_ERR(err) \
  ((void *)err)

// psuedo-random number generators
uint64_t rand(void);

// memory
void * memset(void * dest, char val, size_t count);
void * memcpy(void * dest, const void * src, size_t count);
void * memmove(void * dest, const void * src, size_t n);

// strings
char * itoa(int value, char * str, int radix);
size_t strlen(const char * str);
void reverse(char * str, size_t sz);
void strncpy(char * dest, const char * src, size_t n);
char * strcat(char * dest, const char * src);
char * strdup(char * str);

// printf
int vsnprintf(char * str, size_t len, const char * format, va_list arg);
int vsprintf(char * str, char * format, va_list arg);
int vprintf(char * format, va_list arg);

#endif
