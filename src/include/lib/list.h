#ifndef __LIST_H
#define __LIST_H

struct list_element
{
  void * data;
  struct list_element * next;
};

struct list
{
  int head;
  int tail;
  struct list_element * elements[];
};

#endif
