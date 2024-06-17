#include <jamix/device.h>
#include <jamix/tty.h>
#include <arch/x86.h>
#include <jamix/printk.h>
#include <sys/io.h>
#include <lib/common.h>

#define KEYBOARD_IRQ  33

#define KEYBOARD_DATA_PORT      0x60
#define KEYBOARD_COMMAND_REG    0x64

#define KEYBOARD_STATUS_OUTPUT    BIT(0)
#define KEYBOARD_STATUS_INPUT     BIT(1)
#define KEYBOARD_STATUS_SYSTEM    BIT(2)
#define KEYBOARD_STATUS_COMMAND   BIT(3)
#define KEYBOARD_STATUS_TIMEOUT   BIT(6)
#define KEYBOARD_STATUS_PARITY    BIT(7)

#define HAS_STATUS(stat) (inb(KEYBOARD_COMMAND_REG) & stat)

#define WAIT_FOR_INPUT_STATUS while(!HAS_STATUS(KEYBOARD_STATUS_INPUT))

#define SHIFT    0x1
#define CTRL     0x2
#define ALT      0x4
#define E0ESC    0x8
#define CAPSLOCK 0x10
#define KEY_HOME        0xE0
#define KEY_END         0xE1
#define KEY_UP          0xE2
#define KEY_DN          0xE3
#define KEY_LF          0xE4
#define KEY_RT          0xE5
#define KEY_PGUP        0xE6
#define KEY_PGDN        0xE7
#define KEY_INS         0xE8
#define KEY_DEL         0xE9

static uint8_t keybd_map[256] = {
  0,  0x1B, '1',  '2',  '3',  '4',  '5',  '6',  // 0x00
  '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
  'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  // 0x10
  'o',  'p',  '[',  ']',  '\n', 0,   'a',  's',
  'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  // 0x20
  '\'', '`',  0,  '\\', 'z',  'x',  'c',  'v',
  'b',  'n',  'm',  ',',  '.',  '/',  0,  '*',  // 0x30
  0,  ' ',  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  '7',  // 0x40
  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3',  '0',  '.',  0,  0,  0,  0,   // 0x50
  [0x9C] '\n',      // KP_Enter
  [0xB5] '/',       // KP_Div
  [0xC8] KEY_UP,    [0xD0] KEY_DN,
  [0xC9] KEY_PGUP,  [0xD1] KEY_PGDN,
  [0xCB] KEY_LF,    [0xCD] KEY_RT,
  [0x97] KEY_HOME,  [0xCF] KEY_END,
  [0xD2] KEY_INS,   [0xD3] KEY_DEL
};

static uint8_t shift_map[256] = {
  0,  033,  '!',  '@',  '#',  '$',  '%',  '^',  // 0x00
  '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
  'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  // 0x10
  'O',  'P',  '{',  '}',  '\n', 0,  'A',  'S',
  'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',  // 0x20
  '"',  '~',  0,  '|',  'Z',  'X',  'C',  'V',
  'B',  'N',  'M',  '<',  '>',  '?',  0,  '*',  // 0x30
  0,  ' ',  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  '7',  // 0x40
  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3',  '0',  '.',  0,  0,  0,  0,  // 0x50
  [0x9C] '\n',      // KP_Enter
  [0xB5] '/',       // KP_Div
  [0xC8] KEY_UP,    [0xD0] KEY_DN,
  [0xC9] KEY_PGUP,  [0xD1] KEY_PGDN,
  [0xCB] KEY_LF,    [0xCD] KEY_RT,
  [0x97] KEY_HOME,  [0xCF] KEY_END,
  [0xD2] KEY_INS,   [0xD3] KEY_DEL
};

static uint32_t mode = 0;

/* translate into flags which indicated the satus of shift, ctrl & alt. */
char get_shift_flags(uint8_t scancode)
{
  char ch = scancode & 0x7f;

  if (mode & E0ESC)
  {
    switch (ch)
    {
      case 0x1d:
        return CTRL;
      case 0x38:
        return ALT;
    }
  }
  else
  {
    switch (ch)
    {
      case 0x2a:
      case 0x36:
        return SHIFT;
      case 0x1d:
        return CTRL;
      case 0x38:
        return ALT;
    }
  }
  return 0;
}

void do_keybd_intr(struct interrupt_frame * reg)
{
  uint8_t scancode, ch, m;
  uint8_t * map = keybd_map;

  // got no data
  if ((inb(KEYBOARD_COMMAND_REG) & BIT(0)) == 0)
  {
    return;
  }
  scancode = inb(KEYBOARD_DATA_PORT);

  // ignore capslock yet.
  if ((scancode & 0x7f) == 0x3A)
    return;

  // check E0ESC
  if (scancode == 0xE0)
    mode |= E0ESC;

  // check shift, ctrl and alt
  if ((m = get_shift_flags(scancode)))
  {
    if (scancode & 0x80)
      mode &= ~m;
    else
      mode |= m;
    return;
  }
  map = (mode & SHIFT) ? shift_map : keybd_map;
  ch = map[scancode & 0x7f];

  if (mode & CTRL)
  {
    switch (ch)
    {
      case 'c':
        ch = VINTR;
        break;
      case 'd':
        ch = VEOF;
        break;
      case 'x':
        ch = VKILL;
        break;
      case 'q':
        ch = VSTART;
        break;
      case 's':
        ch = VSTOP;
        break;
      case 'z':
        ch = VSUSP;
        break;
      case '\\':
        ch = VQUIT;
        break;
      default:
        break;
    }
  }

  // on pressed
  if ((scancode & 0x80) == 0 && ch)
  {
    printk("%c", ch);
  }
  // on released
  else
  {
    mode &= ~E0ESC;
  }
  return;
}

void input_init(void)
{
  interrupt_install(&do_keybd_intr, 33);
  printk("Initialized keyboard device\n");
}
