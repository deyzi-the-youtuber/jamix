#include <arch/ops.h>
#include <jamix/acpi.h>
#include <jamix/printk.h>
#include <lib/common.h>
#include <stdint.h>

void apic_init(struct acpi_madt * madt)
{
  if(madt->flags & BIT(0))
    printk("APIC: Dual legacy PIC-8259s detected in the system.\n");
}
