#ifndef __ACPI_H
#define __ACPI_H

#include <jamix/compiler.h>
#include <stdint.h>

/* Basic ACPI */

struct acpi_rsdp
{
  char signature[8];
  uint8_t checksum;
  char oem_id[6];
  uint8_t revision;
  uint32_t rsdt_addr;
}_packed_;

struct acpi_sdt_hdr
{
  char signature[4];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char oem_id[6];
  char oem_table_id[8];
  uint32_t oem_revision;
  uint32_t creator_id;
  uint32_t creator_revision;
};

struct acpi_rsdt
{
  struct acpi_sdt_hdr hdr;
  uint32_t sdt_ptr[];
};

/* FADT */

struct acpi_gas
{
  uint8_t AddressSpace;
  uint8_t BitWidth;
  uint8_t BitOffset;
  uint8_t AccessSize;
  uint64_t Address;
};

struct acpi_fadt
{
  struct acpi_sdt_hdr header;
  uint32_t FirmwareCtrl;
  uint32_t Dsdt;

  // field used in ACPI 1.0; no longer in use, for compatibility only
  uint8_t Reserved;

  uint8_t PreferredPowerManagementProfile;
  uint16_t SCI_Interrupt;
  uint32_t smi_cmd_port;
  uint8_t acpi_enable_cmd;
  uint8_t acpi_disable_cmd;
  uint8_t S4BIOS_REQ;
  uint8_t PSTATE_Control;
  uint32_t PM1aEventBlock;
  uint32_t PM1bEventBlock;
  uint32_t PM1aControlBlock;
  uint32_t PM1bControlBlock;
  uint32_t PM2ControlBlock;
  uint32_t PMTimerBlock;
  uint32_t GPE0Block;
  uint32_t GPE1Block;
  uint8_t PM1EventLength;
  uint8_t PM1ControlLength;
  uint8_t PM2ControlLength;
  uint8_t PMTimerLength;
  uint8_t GPE0Length;
  uint8_t GPE1Length;
  uint8_t GPE1Base;
  uint8_t CStateControl;
  uint16_t WorstC2Latency;
  uint16_t WorstC3Latency;
  uint16_t FlushSize;
  uint16_t FlushStride;
  uint8_t DutyOffset;
  uint8_t DutyWidth;
  uint8_t DayAlarm;
  uint8_t MonthAlarm;
  uint8_t Century;

  // reserved in ACPI 1.0; used since ACPI 2.0+
  uint16_t BootArchitectureFlags;

  uint8_t Reserved2;
  uint32_t Flags;

  // 12 byte structure; see below for details
  struct acpi_gas reset_reg;

  uint8_t ResetValue;
  uint8_t Reserved3[3];
  
  // 64bit pointers - Available on ACPI 2.0+
  uint64_t X_FirmwareControl;
  uint64_t X_Dsdt;

  struct acpi_gas X_PM1aEventBlock;
  struct acpi_gas X_PM1bEventBlock;
  struct acpi_gas X_PM1aControlBlock;
  struct acpi_gas X_PM1bControlBlock;
  struct acpi_gas X_PM2ControlBlock;
  struct acpi_gas X_PMTimerBlock;
  struct acpi_gas X_GPE0Block;
  struct acpi_gas X_GPE1Block;
};

/* MADT */

struct acpi_madt
{
	struct acpi_sdt_hdr sdt;
	uint32_t lapic_addr;
	uint32_t flags;
	uint32_t entry_ptrs[];
}_packed_;

void acpi_init(uint32_t mbi_addr);

#endif
