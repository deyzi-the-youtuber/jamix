#include <arch/ops.h>
#include <jamix/console.h>
#include <jamix/device.h>
#include <jamix/tty.h>
#include <jamix/mem.h>
#include <jamix/errno.h>
#include <lib/common.h>
#include <sys/io.h>
#include <font/vga8x16.h>
#include <multiboot2.h>
#include <stdint.h>

static struct vc_data c;
static struct consw cb;

static int fbcon_x = 0;
static int fbcon_y = 0;

void fbcon_putc(char c);
void fbcon_clear(void);
int fbcon_output_intr(struct tty_struct * tty, size_t len);

static struct device fbcon_dev = {
  .init_name        = "console",
  .device_type      = DEVICE_TYPE_CHAR,
  .device_class     = DEVICE_CLASS_CONSOLE,
  .removable        = false,
  .parent           = NULL,
  .tty_output_intr  = fbcon_output_intr
};

/* startup, and return name */
static char * fbcon_startup(void)
{
  /* use defaults */
  c.vc_num = 0;
  c.vc_size_row = c.vc_rows * 4 * 16;
  c.vc_def_color = 0x07;
  c.vc_attr = c.vc_def_color;
  c.vc_has_color = true;
  c.vc_pos = 0x0000;                        /* sets cursor to (0, 0) */
  cb.con_putc = fbcon_putc;
  cb.con_clear = fbcon_clear;
  return "vga";
}

static uint32_t rgb_to_hex(uint8_t r, uint8_t g, uint8_t b)
{
  return (uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b;
}

static void fbcon_putpx(int x, int y, int r, int g, int b)
{
  uint32_t * buf = (uint32_t *)c.vc_screenbuf;
  uint32_t offset = y * c.vc_rows + x;
  buf[offset] = rgb_to_hex(r, g, b);
}

static void fbcon_print_glyph(int con_x, int con_y, uint8_t * glyph)
{
  int x = con_x * 8;
  int y = con_y * 16;
  for (int dy = 0; dy < 16; dy++) 
  {
    for (int dx = 0; dx < 8; dx++)
    {
      int color = (glyph[dy] >> (7 - dx)) & 1;
      fbcon_putpx(x + dx, y + dy, color ? 0xaa : 0, color ? 0xaa : 0, color ? 0xaa : 0);
    }
  }
}

static void fbcon_scroll(void)
{
  if (fbcon_x > (c.vc_rows / 8) - 1)
  {
    fbcon_x = 0;
    fbcon_y++;
  }
  if (fbcon_y > (c.vc_cols / 16) - 1) // Check if the cursor is at the last row
  {
    // Calculate the size of a single row in bytes
    size_t row_size_bytes = c.vc_rows * 4 * 16;

    // Calculate the size of all rows except the last one
    size_t all_rows_except_last_size = (c.vc_cols / 16) * row_size_bytes;

    // Move all rows up by one (excluding the first row)
    memmove((uint8_t *)c.vc_screenbuf, (uint8_t *)c.vc_screenbuf + row_size_bytes, all_rows_except_last_size);

    // Clear the last row
    memset((uint8_t *)c.vc_screenbuf + all_rows_except_last_size, 0, row_size_bytes);

    // Move the cursor up by one row
    fbcon_y--;
  }
}

void fbcon_putc(char ch)
{
  switch(ch)
  {
    case '\n':
    {
      fbcon_y++;
      fbcon_x = 0;
      break;
    }
    case 8:
    {
      break;
    }
    default:
    {
      uint8_t * glyph = &font8x16[ch * 16];
      fbcon_print_glyph(fbcon_x, fbcon_y, glyph);
      fbcon_x++;
      break;
    }
  }
  fbcon_scroll();
}

void fbcon_write(const char * buf, size_t len)
{
  for(int i = 0; i < len; i++)
  {
    if(buf[i])
      fbcon_putc(buf[i]);
    else
      break;
  }
}

int fbcon_output_intr(struct tty_struct * tty, size_t len)
{
  if(!tty)
    return -EINVAL;
  char * str = (char *)malloc(len);
  if(!str)
    return -ENOMEM;
  ring_buffer_read(tty->write_q, (uint8_t *)str, len);
  fbcon_write(str, len);
  free(str);
  return 0;
}

void fbcon_clear(void)
{

}

static inline void fbcon_cursor_show(void)
{
  uint8_t * glyph = &font8x16[' ' * 16];
  fbcon_print_glyph(fbcon_x, fbcon_y, glyph);
}

static inline void fbcon_cursor_hide(void)
{
  uint8_t * glyph = &font8x16['_' * 16];
  fbcon_print_glyph(fbcon_x, fbcon_y, glyph);
}

void fbcon_cursor_blink(void)
{
  static bool show_cursor = true;
  /* sorry for the unreadable code */
  show_cursor ? fbcon_cursor_show() : fbcon_cursor_hide();
  show_cursor = show_cursor ? false : true;
}

int fbcon_init(struct console * con, uint32_t addr)
{
  struct multiboot_tag_framebuffer_common * grub_fb = (struct multiboot_tag_framebuffer_common *)multiboot2_locate_tag(addr, MULTIBOOT_TAG_TYPE_FRAMEBUFFER);
  if(!grub_fb)
    return -ENODEV;
  c.vc_rows = grub_fb->framebuffer_width;
  c.vc_cols = grub_fb->framebuffer_height;
  c.vc_screenbuf = (uintptr_t)grub_fb->framebuffer_addr;
  cb.con_startup = fbcon_startup;
  strncpy(con->name, cb.con_startup(), sizeof(con->name));
  con->write = fbcon_write;
  con->data = &c;
  con->dev = &fbcon_dev;
  create_callback(fbcon_cursor_blink, 25); /* create callback that triggers every 250 ms */
  return 0;
}
