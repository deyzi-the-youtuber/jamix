#include <jamix/mem.h>
#include <jamix/errno.h>
#include <jamix/termios.h>
#include <jamix/printk.h>
#include <jamix/tty.h>
#include <lib/common.h>
#include <sys/types.h>

struct tty_struct * ttys[NR_CONSOLES] = {0};

struct tty_struct * tty_lookup(int num)
{
  if(!ttys[num])
    return PTR_ERR(-ENODEV);
  return ttys[num];
}

static void termios_init(struct termios * termios)
{
  termios->c_iflag = BRKINT | ICRNL | IXON | IXANY;
  termios->c_oflag = OPOST | ONLCR;
  termios->c_cflag = CREAD | CS8 | HUPCL;
  termios->c_lflag = ISIG | IEXTEN | ICANON | ECHO | ECHOE;
#define CONTROL(char) (((char) - 64) & 0x7f)
  termios->c_cc[VEOF] = CONTROL('D');
  termios->c_cc[VEOL] = CONTROL('?');
  termios->c_cc[VERASE] = '\b';
  termios->c_cc[VINTR] = CONTROL('C');
  termios->c_cc[VKILL] = CONTROL('U');
  termios->c_cc[VMIN] = 1;
  termios->c_cc[VQUIT] = CONTROL('\\');
  termios->c_cc[VSTART] = CONTROL('Q');
  termios->c_cc[VSTOP] = CONTROL('S');
  termios->c_cc[VSUSP] = CONTROL('Z');
  termios->c_cc[VTIME] = 0;
}

#define IS_VALID_TTY(tty) \
  (tty && tty->dev->device_type == DEVICE_TYPE_CHAR && tty->dev->device_class == DEVICE_CLASS_CONSOLE)

size_t tty_write(struct tty_struct * tty, const uint8_t * buf, size_t count)
{
  if(!IS_VALID_TTY(tty))
    return -EINVAL;

  if(!tty->ops->write)
    return -EIO;

  if(!tty->dev)
  {
    printk("tty_write: device is null\n");
    return -ENODEV; 
  }

  char * str = (char *)malloc(count + 1);
  size_t i;

  if(!str)
    return -ENOMEM;
  strncpy(str, (char *)buf, count);

  for(i = 0; str[i]; i++)
    ring_buffer_write(tty->write_q, str[i]);

  tty->dev->tty_output_intr(tty, i);
  free(str);
  return i;
}

static void tty_release(struct tty_struct * tty)
{
  memset((void *)tty, 0, sizeof(struct tty_struct));
  free(tty);
}

int tty_create(int num, struct device * dev)
{
  if(ttys[num])
  {
    return -EPERM;
  }
  struct tty_struct * tty = (struct tty_struct *)malloc(sizeof(struct tty_struct));
  if(!tty)
  {
    tty_release(tty);
    return -ENOMEM;
  }
  memset((void *)tty, 0, sizeof(struct tty_struct));
  memset((void *)&tty->termios, 0, sizeof(struct termios));
  termios_init(&tty->termios);
  if (ring_buffer_init(tty->write_q, TTY_BUF_SIZE) < 0)
  {
    goto ring_mem_err;
  }
  if(ring_buffer_init(tty->read_q, TTY_BUF_SIZE) < 0)
  {
    goto ring_mem_err;
  }
  tty->winsize.ws_row = console_get_rows();
  tty->winsize.ws_col = console_get_cols();
  tty->winsize.ws_xpixel = 0;
  tty->winsize.ws_ypixel = 0;
  tty->ops->write = tty_write;
  tty->dev = dev;
  ttys[num] = tty;
  return 0;
ring_mem_err:
  tty_release(tty);
  return -ENOMEM;
}
