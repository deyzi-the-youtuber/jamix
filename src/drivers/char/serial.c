#include <jamix/errno.h>
#include <jamix/mem.h>
#include <jamix/device.h>
#include <jamix/tty.h>
#include <jamix/printk.h>
#include <jamix/fs.h>
#include <sys/io.h>
#include <sys/types.h>
#include <lib/common.h>
#include <stdint.h>

#define STATUS_REG_THRE BIT(5)

struct serial_dev
{
  uint16_t port;
  bool faulty;
};

// assume that every serial device
// is a good serial device
static struct serial_dev serial_ports[9] =
{
  {0x3f8, false},
  {0x2f8, false},
  {0x3e8, false},
  {0x2e8, false},
  {0x5f8, false},
  {0x4f8, false},
  {0x5e8, false},
  {0x4e8, false},
  {0, true}
};

static int serial_dev_init(uint16_t port)
{
  outb(serial_ports[port].port + 1, 0x00); /* disable interrupts */ 
  outb(serial_ports[port].port + 3, 0x80); /*  */
  outb(serial_ports[port].port, 0x03);    
  outb(serial_ports[port].port + 1, 0x00);
  outb(serial_ports[port].port + 3, 0x03);    
  outb(serial_ports[port].port + 2, 0xc7);
  outb(serial_ports[port].port + 4, 0x0b);
  outb(serial_ports[port].port + 4, 0x1e);
  outb(serial_ports[port].port, '!');
  if(inb(serial_ports[port].port) != '!')
  {
    return -EIO;
  }
  outb(serial_ports[port].port + 4, 0x0f);
  return 0;
}

void serial_init(void)
{
  for(int i = 0; serial_ports[i].port; i++)
  {
    int rc = serial_dev_init(i);
    if(rc < 0)
      serial_ports[i].faulty = true;
  }
}

static inline bool serial_empty_transit(int port)
{
  return inb(serial_ports[port].port + 5) & STATUS_REG_THRE;
}

static int _serial_write(int port, const char * str, size_t sz)
{
  if(serial_ports[port].faulty)
    return -EIO;
  for(int i = 0; i < sz; i++)
  {
    while (!serial_empty_transit(port));
    switch(str[i])
    {
      case '\n':
        outb(serial_ports[port].port, '\r');
        outb(serial_ports[port].port, str[i]);
        break;
      default:
        outb(serial_ports[port].port, str[i]);
        break;
    }
  }
  return 0;
}

int serial_dev_write(struct file * file, void * buf, size_t sz)
{
  return _serial_write(0, (const char *)buf, sz);
}

void debug(const char * fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char * buf = (char *)malloc(256);
  vsprintf(buf, (char *)fmt, ap);
  va_end(ap);
  serial_dev_write(NULL, (void *)buf, strlen(buf));
  free(buf);
}

struct file_operations serial_dev_ops = {
  .read = NULL,
  .write = serial_dev_write,
  .readdir = NULL,
  .ioctl = NULL,
  .open = NULL,
  .close = NULL,
};

struct file serial_dev_file = {
  .mode = 0,
  .pos = 0,
  .flags = 0,
  .count = 0,
  .name = "serial",
  .path = "/dev/serial",
  .ops = &serial_dev_ops
};
