#include <stdint.h>
#include <sys/io.h>

#define PIC_MASTER_COMMAND_PORT 0x0020
#define PIC_MASTER_DATA_PORT    0x0021
#define PIC_SLAVE_COMMAND_PORT  0x00a0
#define PIC_SLAVE_DATA_PORT     0x00a1

#define PIC_EOI	0x20
#define ICW1_ICW4	0x01
#define ICW1_SINGLE	0x02
#define ICW1_INTERVAL4	0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT	0x10
 
#define ICW4_8086	0x01
#define ICW4_AUTO	0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER	0x0C
#define ICW4_SFNM	0x10

static void pic_master_send_cmd(uint8_t cmd)
{
  outb(PIC_MASTER_COMMAND_PORT, cmd);
}

static void pic_master_send_dat(uint8_t data)
{
  outb(PIC_MASTER_DATA_PORT, data);
}

static void pic_slave_send_cmd(uint8_t cmd)
{
  outb(PIC_SLAVE_COMMAND_PORT, cmd);
}

static void pic_slave_send_dat(uint8_t data)
{
  outb(PIC_SLAVE_DATA_PORT, data);
}

uint8_t read_pic_master_data(void)
{
  return inb(PIC_MASTER_DATA_PORT);
}

uint8_t read_pic_slave_data(void)
{
  return inb(PIC_SLAVE_DATA_PORT);
}

static void io_wait(void)
{
  outb(0x80, 0);
}

void pic_eoi(int intr)
{
	if(intr >= 8)
		pic_slave_send_cmd(PIC_EOI);
	pic_master_send_cmd(PIC_EOI);
}

void pic_mask(int intr)
{
  uint16_t port;
  uint8_t value;
 
  if(intr < 8)
  {
    port = PIC_MASTER_DATA_PORT;
  } 
  else 
  {
    port = PIC_SLAVE_DATA_PORT;
    intr -= 8;
  }
  value = inb(port) | (1 << intr);
  outb(port, value);        
}
 
void pic_unmask(int intr)
{
  uint16_t port;
  uint8_t value;
 
  if(intr < 8)
  {
    port = PIC_MASTER_DATA_PORT;
  } 
  else 
  {
    port = PIC_SLAVE_DATA_PORT;
    intr -= 8;
  }
  value = inb(port) & ~(1 << intr);
  outb(port, value);        
}

void pic_remap_vectors(void)
{
	uint8_t a1, a2;
 
	a1 = read_pic_slave_data();
	a2 = read_pic_master_data();
 
	pic_master_send_cmd(ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	pic_slave_send_cmd(ICW1_INIT | ICW1_ICW4);
	io_wait();
	pic_master_send_dat(0x20);                 // ICW2: Master PIC vector offset
	io_wait();
	pic_slave_send_dat(0x28);                 // ICW2: Slave PIC vector offset
	io_wait();
	pic_master_send_dat(4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	pic_slave_send_dat(2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	pic_master_send_dat(ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	io_wait();
	pic_slave_send_dat(ICW4_8086);
	io_wait();
 
	pic_master_send_dat(a1);   // restore saved masks.
	pic_slave_send_dat(a2);
  /* UEFI masks these interrupts. I don't know why. */
  for(int i = 0; i < 42; i++)
    pic_unmask(i);
}
