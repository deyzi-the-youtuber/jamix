#include <stdint.h>
#include <arch/ops.h>
#include <jamix/console.h>
#include <jamix/printk.h>
#include <jamix/mem.h>
#include <jamix/device.h>
#include <jamix/errno.h>
#include <jamix/tty.h>
#include <jamix/init.h>
#include <jamix/process.h>
#include <lib/common.h>
#include <multiboot2.h>

extern uintptr_t kernel_start;
extern uintptr_t kernel_end;

extern uintptr_t bss_start;
extern uintptr_t bss_end;

uint32_t mbi_addr;

#define tty0_write(str) tty_write(tty_lookup(0), str, strlen(str))

static void bss_init(void)
{
  memset(&bss_start, 0, &bss_end - &bss_start);
}

void _start(uint32_t magic, uint32_t addr)
{
  int rc = 0;
  if(addr & 7)
    for(;;);
  bss_init();
  /* early arch stuff. well be fine with some things for a bit */
  arch_early_init();
  heap_init(&kernel_end);
  rc = console_init(addr);
  if(rc < 0)
  {
    critical_enter();
    for(;;);
  }
  printk("Jamix kernel version 1.00\n");
  printk("Copyright (c) 2024 Jaime Raul Garza. All Rights Reserved.\n");
  serial_init();
  rc = tty_create(0, con.dev);
  if(IS_ERR(rc))
    panic("failed to create tty0: %d\n", rc);
  sched_init();
  for(;;);
}
