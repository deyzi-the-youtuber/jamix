#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <jamix/tty.h>

int keyboard_init(void);
int keyboard_assign_tty(struct tty_struct * tty_new);

#endif
