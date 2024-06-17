#include <stdint.h>
#include <jamix/console.h>
#include <lib/common.h>
#include <sys/io.h>
#include <jamix/device.h>
#include <jamix/tty.h>
#include <jamix/mem.h>
#include <jamix/errno.h>

#define VGACON_ROWS 80
#define VGACON_COLS 25

static struct vc_data c;
static struct consw cb;

static int vgacon_x = 0;
static int vgacon_y = 0;

void vgacon_putc(char c);
void vgacon_clear(void);
void vgacon_cur_remap(uint16_t pos);
int vgacon_output_intr(struct tty_struct * tty, size_t len);

struct device vgacon_dev = {
  .init_name        = "console",
  .device_type      = DEVICE_TYPE_CHAR,
  .device_class     = DEVICE_CLASS_CONSOLE,
  .removable        = false,
  .parent           = NULL,
  .tty_output_intr  = vgacon_output_intr
};

/* startup, and return name */
char * vgacon_startup(void)
{
  /* use defaults */
  c.vc_num = 0;
  c.vc_rows = VGACON_ROWS;
  c.vc_cols = VGACON_COLS;
  c.vc_size_row = VGACON_ROWS * 2;
  c.vc_def_color = 0x07;
  c.vc_attr = c.vc_def_color;
  c.vc_has_color = true;
  c.vc_screenbuf = (uintptr_t)0xb8000;
  c.vc_pos = 0x0000;                        /* sets cursor to (0, 0) */
  cb.con_putc = vgacon_putc;
  cb.con_clear = vgacon_clear;
  return "vga";
}

static inline void bs(void)
{
}

static void vgacon_scroll(void)
{
  if (vgacon_x > (c.vc_rows - 1))
  {
    vgacon_x = 0;
    vgacon_y++;
  }
  if (vgacon_y >= c.vc_cols)
  {
    for (int i = 1; i < c.vc_cols; i++)
    {
      memmove((void *)c.vc_screenbuf + (i - 1) * c.vc_size_row, (void *)c.vc_screenbuf + i * c.vc_size_row, c.vc_size_row);
    }
    char * last_row = (void *)c.vc_screenbuf + 24 * c.vc_size_row;
    memset(last_row, 0, c.vc_size_row);
    vgacon_y--;
  }
}

#define PUT_CHAR(ch) (uint16_t)(ch | (c.vc_attr << 8))
#define CURSOR_POS(x, y) (y * c.vc_rows + x)

void vgacon_putc(char ch)
{
  uint16_t * mem = (uint16_t *)(c.vc_screenbuf + CURSOR_POS(vgacon_x, vgacon_y) * 2);
  switch(ch)
  {
    case '\n':
    {
      vgacon_y++;
      vgacon_x = 0;
      break;
    }
    case 8:
    {
      bs();
      break;
    }
    default:
    {
      *mem = PUT_CHAR(ch);
      vgacon_x++;
      break;
    }
  }
  vgacon_scroll();
  vgacon_cur_remap(CURSOR_POS(vgacon_x, vgacon_y));
}

void vgacon_cur_remap(uint16_t pos)
{
  outb(0x3d4, 0x0f);
  outb(0x3d5, (uint8_t)(pos & 0xff));
  outb(0x3d4, 0x0e);
  outb(0x3d5, (uint8_t)((pos >> 8) & 0xff));
}

void vgacon_clear(void)
{
  /* not implemented */
}

void vgacon_write(const char * buf, size_t len)
{
  for(int i = 0; i < len; i++)
  {
    if(buf[i])
      vgacon_putc(buf[i]);
    else
      break;
  }
}

int vgacon_output_intr(struct tty_struct * tty, size_t len)
{
  if(!tty)
    return -EINVAL;
  char * str = (char *)malloc(len);
  if(!str)
    return -ENOMEM;
  ring_buffer_read(tty->write_q, (uint8_t *)str, len);
  vgacon_write(str, len);
  free(str);
  return 0;
}

int vgacon_init(struct console * con)
{
  cb.con_startup = vgacon_startup;
  strncpy(con->name, cb.con_startup(), sizeof(con->name));
  con->write = vgacon_write;
  con->data = &c;
  con->dev = &vgacon_dev;
  return 0;
}
