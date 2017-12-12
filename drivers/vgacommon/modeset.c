#include <modeset.h>

//Based on 
//http://files.osdev.org/mirrors/geezer/osd/graphics/modes.c

//                          VGA 3h mode
unsigned char vga_3h_mode[] =
{
       // MISC 
	0x67,
       // SEQ 
	0x03, 0x00, 0x03, 0x00, 0x02,
       // CRTC 
	0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
	0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50,
	0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
	0xFF,
       // GC 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
	0xFF,
       // AC 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00
};


//                          VGA 13h mode
//=========================================================
unsigned char vga_13h_mode[] =
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

//Give this one of the arrays above and
//it will set the given video mode.
void set_vga_mode(unsigned char *regs)
{
  uint8_t i;
  // write MISCELLANEOUS reg 
  portb_write(VGA_MISC_P, *regs);
  regs++;

  // Set the sequencer registers 
  for(i = 0; i < 5; i++){
      portb_write( VGA_SEQ_IDX_P, i);
      portb_write( VGA_SEQ_DATA_P, *regs);
      regs++;
  }

  // Unlock the crtc registers 
  // so we may write the register values 
  portb_write(VGA_SEQ_IDX_P, 0x03);
  portb_write(VGA_CRTC_DATA_P, portb_read(VGA_CRTC_DATA_P) | 0x80);
  portb_write(VGA_CRTC_IDX_P, 0x11);
  portb_write(VGA_CRTC_DATA_P, portb_read(VGA_CRTC_DATA_P) & ~0x80);

  //Make sure the registers remain unlocked
  regs[0x03] |= 0x80;
  regs[0x11] &= ~0x80;

  // Set the crtc regiseters, now that 
  // we have unlocked them 
  for(i = 0; i < 25; i++){
      portb_write(VGA_CRTC_IDX_P, i);
      portb_write(VGA_CRTC_DATA_P, *regs);
      regs++;
  }


  // Set the graphics controller registers
  for(i = 0; i < 9; i++){
      portb_write(VGA_GRAPHICSC_IDX_P, i);
      portb_write(VGA_GRAPHICSC_DATA_P, *regs);
      regs++;
  }
  
  // Set the attribute controller registers 
  for(i = 0; i < 21; i++){
      (void)portb_read(0x3DA);
      portb_write(VGA_ATTRC_DATA_P, i);
      portb_write(VGA_ATTRC_DATA_P, *regs);
      regs++;
  }
  
  // lock 16-color palette and unblank display
  (void)portb_read(0x3DA);
  portb_write(VGA_ATTRC_DATA_P, 0x20);
}

//Sets the given VGA plane
//Taken from http://files.osdev.org/mirrors/geezer/osd/graphics/modes.c
void set_plane(uint8_t plane){
   uint8_t plane_mask;
   
   plane &= 3;
   plane_mask = 1 << plane;

   //Set read plane
   portb_write( VGA_GRAPHICSC_IDX_P, 4 );
   portb_write( VGA_GRAPHICSC_DATA_P, plane);

   //Set write plane
   portb_write( VGA_SEQ_IDX_P, 2);
   portb_write( VGA_SEQ_DATA_P, plane_mask );
}
