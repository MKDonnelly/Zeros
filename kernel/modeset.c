#include <modeset.h>

//Much of this taken from 
//http://files.osdev.org/mirrors/geezer/osd/graphics/modes.c

//                          VGA 13h mode
//=========================================================

/*
#define   VGA13H_BLACK    0,
#define   VGA13H_BLUE     1
#define   VGA13H_GREEN    2
#define   VGA13H_CYAN     3
#define   VGA13H_RED      4
#define   VGA13H_MAGENTA  5
#define   VGA13H_BROWN    6
#define   VGA13H_LGREY    7
#define   VGA13H_DGREY    8
#define   VGA13H_LBLUE    9
#define   VGA13H_LGREEN   10
#define   VGA13H_LCYAN    11
#define   VGA13H_LRED     12
#define   VGA13H_LMAGENTA 13
#define   VGA13H_YELLOW   14
#define   VGA13H_WHITE    15
*/

unsigned char vga_13h_regs[] =
{
    // MISC 
    0x63,
    // SEQ 
    0x03, 0x01, 0x0F, 0x00, 0x0E,
    // CRTC 
    0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
    0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9C, 0x0E, 0x8F, 0x28,0x40, 0x96, 0xB9, 0xA3,
    0xFF,
    // GC 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
    0xFF,
    // AC 
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x41, 0x00, 0x0F, 0x00,0x00
};



void write_regs(unsigned char *regs)
{
  unsigned i;

  // write MISCELLANEOUS reg 
  portb_write(0x3C2, *regs);
  regs++;

  // write SEQUENCER regs 
  for(i = 0; i < 5; i++){
      portb_write(0x3C4, i);
      portb_write(0x3C5, *regs);
      regs++;
  }

  // unlock CRTC registers
  portb_write(0x3C4, 0x03);
  portb_write(0x3D5, portb_read(0x3D5) | 0x80);
  portb_write(0x3D4, 0x11);
  portb_write(0x3D5, portb_read(0x3D5) & ~0x80);

  // write CRTC regs 
  for(i = 0; i < 25; i++){
      portb_write(0x3D4, i);
      portb_write(0x3D5, *regs);
      regs++;
  }

  // write GRAPHICS CONTROLLER regs
  for(i = 0; i < 9; i++){
      portb_write(0x3CE, i);
      portb_write(0x3CF, *regs);
      regs++;
  }
  
  // write ATTRIBUTE CONTROLLER regs 
  for(i = 0; i < 21; i++){
      (void)portb_read(0x3DA);
      portb_write(0x3C0, i);
      portb_write(0x3C0, *regs);
      regs++;
  }
  
  // lock 16-color palette and unblank display
  (void)portb_read(0x3DA);
  portb_write(0x3C0, 0x20);
}
