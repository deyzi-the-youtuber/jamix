#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>
#include <sys/types.h>

// jamix heap

void heap_init(uint64_t * start);
uint64_t heap_get_used(void);
void * malloc(size_t n);
void * calloc(size_t num, size_t size);
void free(void * ptr);

#endif
