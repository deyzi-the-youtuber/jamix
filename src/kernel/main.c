#include <arch/ops.h>
#include <jamix/console.h>
#include <jamix/printk.h>
#include <jamix/mem.h>
#include <jamix/device.h>
#include <jamix/errno.h>
#include <jamix/tty.h>
#include <jamix/compiler.h>
#include <jamix/process.h>
#include <jamix/version.h>
#include <jamix/utsname.h>
#include <jamix/limits.h>
#include <jamix/acpi.h>
#include <jamix/params.h>
#include <jamix/keyboard.h>
#include <lib/common.h>
#include <multiboot2.h>
#include <stdint.h>

#include "cmdline.h"

/* defined in link.lds */
extern uintptr_t kernel_start;
extern uintptr_t kernel_end;

extern uintptr_t bss_start;
extern uintptr_t bss_end;

const char * param_rootdisk = NULL;
const char * param_initpath = NULL;

#define tty0_write(str) tty_write(tty_lookup(0), str, strlen(str))

static void bss_init(void)
{
  memset(&bss_start, 0, &bss_end - &bss_start);
}

void _start(uint32_t magic, uint32_t addr)
{
  int rc = 0;
  bss_init();

  if(addr & 7)
    panic_crit();

  /* early arch stuff. we'll be fine with some things for a bit */
  arch_early_init();
  heap_init(&kernel_end);
  rc = console_init(addr);

  if(rc < 0)
    panic_crit();

  printk(jamix_boot_banner, UTS_RELEASE, UTS_VERSION);
  /* get cmdline params */
  char * cmdline = get_cmdline_from_mboot2(addr);
  param_initpath = get_cmdline_param_val(cmdline, "init");
  param_rootdisk = get_cmdline_param_val(cmdline, "root");
  
  printk("Kernel cmdline: %s\n", cmdline);
  printk("Copyright (c) 2024 Jaime Raul Garza. All Rights Reserved.\n");
  acpi_init(addr);
  serial_init();
  rc = tty_create(0, get_console()->dev);
  if(IS_ERR(rc))
    panic("Failed to create tty0: %d\n", rc);
  keyboard_init();
  sched_init();
  for(;;);
}

void kernel_late_init()
{
  printk("This is kernel late init\n");
}
