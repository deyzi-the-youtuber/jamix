#include <stdint.h>
#include <jamix/console.h>
#include <jamix/errno.h>
#include <lib/common.h>
#include <arch/x86.h>

struct console con;

static bool console_enabled = false;

extern int vgacon_init(struct console * con);
extern int fbcon_init(struct console * con, uint32_t addr);

uint32_t console_get_rows(void)
{
  return con.data->vc_rows;
}

uint32_t console_get_cols(void)
{
  return con.data->vc_cols;
}

inline bool is_console_enabled(void)
{
  return console_enabled;
}

inline int console_puts(char * buf)
{
  if(!con.write)
    return -EIO;
  con.write(buf, strlen(buf));
  return 0;
}

int console_init(uint32_t addr)
{
  int rc;
  rc = fbcon_init(&con, addr);
  if(rc < 0)
    return rc;
  console_enabled = true;
  return 0;
}
