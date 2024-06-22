#ifndef __PCI_H
#define __PCI_H

#include <lib/common.h>
#include <stdint.h>

/* common pci fields */
#define PCI_VENDOR_ID         0x00
#define PCI_DEVICE_ID         0x02
#define PCI_COMMAND           0x04
#define PCI_STATUS            0x06
#define PCI_REVISION          0x08
#define PCI_PROGIF            0x09
#define PCI_DEVICE_CLASS      0x0a
#define PCI_CACHE_LINE_SIZE   0x0c
#define PCI_DEVICE_LATENCY    0x0d
#define PCI_HEADER_TYPE       0x0e
#define PCI_BIST              0x0f
/* header type 0x0 extensions */
#define PCI_BAR0              0x10
#define PCI_BAR1              0x14
#define PCI_BAR2              0x18
#define PCI_BAR3              0x1c
#define PCI_BAR4              0x20
#define PCI_BAR5              0x24
#define PCI_CARDBUS_CIS_PTR   0x28
#define PCI_SUBSYS_VENDOR_ID  0x2c
#define PCI_SUBSYS_ID         0x2e
#define PCI_ROM_ADDRESS       0x30
#define PCI_INTERRUPT_LINE	  0x3c
#define PCI_INTERRUPT_PIN	    0x3d
#define PCI_MIN_GRANT		      0x3e
#define PCI_MAX_LATENCY		    0x3f
/* class types, etc. */
#define PCI_CLASS_NOCLASS     0x00
#define PCI_CLASS_STORAGE     0x01
#define PCI_CLASS_NETWORK     0x02
#define PCI_CLASS_DISPLAY     0x03
#define PCI_CLASS_MULTIMEDIA  0x04
#define PCI_CLASS_MEMCTRL     0x05
#define PCI_CLASS_BRIDGE      0x06
#define PCI_CLASS_COMM        0x07
#define PCI_CLASS_SYSTEM      0x08
#define PCI_CLASS_INPUT       0x09
#define PCI_CLASS_DOCKING     0x0a
#define PCI_CLASS_PROC        0x0b
#define PCI_CLASS_SBUS        0x0c
#define PCI_CLASS_WIRELESS    0x0d
#define PCI_CLASS_INTELLIGENT 0x0e
#define PCI_CLASS_SATELLITE   0x0f
#define PCI_CLASS_ENCRYPTION  0x10
#define PCI_CLASS_SIGNAL_PROC 0x11
#define PCI_CLASS_PROC_ACCEL  0x12
#define PCI_CLASS_UNASSIGNED  0xff

struct pci_dev_info
{
	uint16_t vendor;		  /* vendor id */
	uint16_t device;		  /* device id */
	const char * name;		/* device name */
	uint8_t	bridge_type;	/* bridge type (0xff if no bridge type) */
};

#endif
