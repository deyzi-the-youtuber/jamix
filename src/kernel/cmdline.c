#include <multiboot2.h>
#include "cmdline.h"

char * get_cmdline_from_mboot2(uint32_t addr)
{
  struct multiboot_tag_string * string_tag = (struct multiboot_tag_string *)multiboot2_locate_tag(addr, MULTIBOOT_TAG_TYPE_CMDLINE);
  return string_tag->string;
}
