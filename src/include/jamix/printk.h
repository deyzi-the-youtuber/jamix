#ifndef __PRINTK_H
#define __PRINTK_H

int printk(char * format, ...);
void panic(char * format, ...);
void panic_crit(void);

#endif
