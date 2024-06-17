#include <multiboot2.h>
#include <lib/common.h>
#include <stdint.h>

void * multiboot2_locate_tag(uint32_t addr, int type)
{
  //debug("multiboot2_locate_tag: attempting to locate tag %d...\n", type);
  struct multiboot_header_tag * tag = (struct multiboot_header_tag *)(uint64_t)(addr + 8);
  while(1)
  {
    if(!tag->type)
      return NULL;
    if(tag->type == type)
      return (void *)tag;

    tag = (struct multiboot_header_tag *)((uint8_t *) tag + ((tag->size + 7) & ~7));
  }
}
