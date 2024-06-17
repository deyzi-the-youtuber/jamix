#include <jamix/mem.h>
#include <jamix/errno.h>
#include <lib/common.h>
#include <lib/list.h>

int list_init(struct list * list)
{
  list = (struct list *)malloc(sizeof(struct list));
  if(!list)
    return -ENOMEM;
  list->head = 0;
  list->tail = 0;
  return 0;
}

int list_element_add(struct list * list, struct list_element * elem)
{
  list->elements[list->tail] = elem;
}
