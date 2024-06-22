#include <jamix/acpi.h>
#include <jamix/printk.h>
#include <lib/common.h>
#include <sys/io.h>
#include <multiboot2.h>
#include <stdint.h>

#define PMA1_CTRL_BLOCK_ACPI_ENABLED BIT(0)

#define WAIT_FOR_ACPI_ENABLE while(inw(fadt->PM1aControlBlock) & PMA1_CTRL_BLOCK_ACPI_ENABLED == 0);

struct acpi_rsdp * rsdp = NULL;
struct acpi_rsdt * rsdt = NULL;
struct acpi_madt * madt = NULL;
struct acpi_fadt * fadt = NULL;

static uint8_t verify_checksum(uint8_t * pointer, size_t size)
{
  uint8_t bytes = 0;
  while(size--)
  {
    bytes += *pointer++;
  }
  return bytes;
}

static void * acpi_find_sdt(char signature[4], struct acpi_rsdt * rsdt)
{
  int entries = (rsdt->hdr.length - sizeof(rsdt->hdr)) / 4;
  for(int i = 0; i < entries; i++)
  {
    struct acpi_sdt_hdr * header = (struct acpi_sdt_hdr *)(uint64_t)rsdt->sdt_ptr[i];
    if(!strncmp(header->signature, signature, 4))
    {
      return (void *)header;
    }
  }
  return NULL;
}

void acpi_init(uint32_t mbi_addr)
{
  struct multiboot_tag_old_acpi * rsdp_tag = multiboot2_locate_tag(mbi_addr, MULTIBOOT_TAG_TYPE_ACPI_OLD);
  if(!rsdp_tag)
  {
    printk("ACPI: RSDP could not be located.\n");
    return;
  }
  rsdp = (struct acpi_rsdp *)rsdp_tag->rsdp;
  printk("ACPI: Location of RSDP @ 0x%016x\n", rsdp);
  rsdt = (struct acpi_rsdt *)(uint64_t)rsdp->rsdt_addr;
  printk("ACPI: Location of RSDT @ 0x%016x\n", rsdt);
  void * madt_ptr = acpi_find_sdt("APIC", rsdt);
  void * fadt_ptr = acpi_find_sdt("FACP", rsdt);
  if(!madt_ptr)
  {
    printk("ACPI: Failed to find MADT\n");
    return;
  }
  if(!fadt_ptr)
  {
    printk("ACPI: Failed to find FADT\n");
    return;
  }
  fadt = (struct acpi_fadt *)fadt_ptr;
  madt = (struct acpi_madt *)madt_ptr;
  printk("ACPI: Found MADT @ 0x%016x\n", madt_ptr);
  printk("ACPI: Local APIC address @ 0x%016x\n", madt->lapic_addr);
  printk("ACPI: Enabling ACPI...\n");
  apic_init(madt);
  outb(fadt->smi_cmd_port, fadt->acpi_enable_cmd);
  printk("ACPI: Successfully enabled ACPI!\n");
}
