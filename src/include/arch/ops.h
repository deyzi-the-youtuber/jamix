#ifndef __ARCH_H
#define __ARCH_H

#include <stdint.h>

int create_callback(void (*cb_func)(void), uint64_t ms);

void arch_early_init(void);
void critical_enter(void);
void critical_exit(void);

#endif
