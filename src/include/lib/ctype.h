#ifndef __CTYPE_H
#define __CTYPE_H

static inline int isspace(int c)
{
	return ((uint8_t)c == ' ') || (uint8_t)(c - 0x09) < 5;
}

#endif
