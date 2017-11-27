#include <portio.h>
#include <timing.h>
#include <serial/serial.h>
#include <cmos.h>
#include <pic.h>
#include <vgatext/vgatext.h>
#include <vga13h/vga13hmode.h>
#include <keyboard.h>

#include <string.h>
#include <bcd.h>
#include <types.h>
#include <bitwise.h>
#include <delay.h>

#include <cpu.h>
#include <isr.h>
#include <idt.h>
#include <gdt.h>
#include <paging.h>
#include <kmalloc.h>

#include <modeset.h>

void kmain(){

  char *vidmem = (char*)0xA0000;
  write_regs( vga_13h_regs );

  for( int i = 0, j = 0; i < 320 * 200; i++, j++){
     vidmem[i] = j;
     if( j >= 255 )
        j = 0;
  }

  for( int k = 0; k < 320 * 2; k++){
     vidmem[k] = 5;
  }

  while(1);
}


/*
void init13h(){

   //Write the general registers
   portb_write( 0x3C2, mode13h[0]);
   portb_write( 0x3BA, mode13h[1]);
   //portb_write( 0x3C2, mode13h[2]);
   //portb_write( 0x3BA, mode13h[3]); 

   //Write to the sequencer
   portb_write( 0x3C4, 0 );
   portb_write( 0x3C5, mode13h[4]);

   portb_write( 0x3C4, 1);
   portb_write( 0x3C5, mode13h[5]);

   portb_write( 0x3C4, 2);
   portb_write( 0x3C5, mode13h[6]);

   portb_write( 0x3C4, 3);
   portb_write( 0x3C5, mode13h[7]);
   
   portb_write( 0x3C4, 4);
   portb_write( 0x3C5, mode13h[8]);

   //Write to the CRTC
   for(int i = 0; i < 25; i++){
      portb_write( 0x3D4, i);
      portb_write( 0x3D5, mode13h[i+9]);
   }
   
   //Write to the graphics registers
   for(int i = 0; i < 9; i++){
      portb_write( 0x3CE, i);
      portb_write( 0x3CF, mode13h[i+34]);
   }

   //Write to the attribute registers
   for(int i = 0; i < 21; i++){
      portb_write( 0x3C0, i);
      portb_write( 0x3C0, mode13h[i+43]);
   }
}
*/


/*
//TODO add in struct multiboot* to 
//get information about the system
void kmain(){



  init_gdt();

  //Initilize the PIC
  remap_pic();
  //Create the IDT and initilize
  //the interrupt handlers
  init_interrupts();
  
  init_keyboard();
  init_timer();
  move_cursorl( k_xy_to_linear( 0, 0 ) );
  k_clear_screen();

  //Make sure to enable 
  //the interrupts
  enable_ints();

  k_newline();
  k_newline();
  k_print("Enter some text: ");

  //Experimental kmalloc
  heap_init();

  char *mem = (char*)kmalloc( 10, 1, 0);
  kfree( mem );

  //Paging
  init_paging();

  //Paging testing
  //page_map(get_page( 0x2000000, 1, kernel_page_dir ), KERNEL_MEMORY, IS_WRITEABLE, 0xb8000);
  //char *vid = (char*)0x2000000;
  // *(vid + 20) = 'z';


  //ALWAYS have this, or else the program
  //will run off the end of the world.
  while(1);
}*/

//         Examples

/*
  //Initilize the GDT
  init_gdt();
  //Initilize the PIC
  remap_pic();
  //Create the IDT and initilize
  //the interrupt handlers
  install_interrupts();
  
  init_keyboard();
  init_timer();
  move_cursorl( k_xy_to_linear( 0, 0 ) );
  k_clear_screen();

  //Make sure to enable 
  //the interrupts
  enable_ints();

  k_newline();
  k_newline();
  k_print("Enter some text: ");

//Paging
  init_paging();

  //Test mapping
  page_map( get_page( 0xB8000, 1, kernel_page_dir), KERNEL_MEMORY, IS_WRITEABLE, 0xB8000);

  //Create a page fault for testing
  char *ptr = (char*)0xB8000;
  *(ptr + 20) = 'Z';
//End paging

*/
/*       Dynamic memory allocation
  char *mystr = (char*)kmalloc(100, 0);
  strcpy( mystr, "Testing..." );
  k_print( mystr );
*/

/*       Using the keyboard
  //Initilize the PIC
  remap_pic();
  //Create the IDT and initilize
  //the interrupt handlers
  install_interrupts();
  
  init_keyboard();
  move_cursorl( k_xy_to_linear( 0, 0 ) );

  //Make sure to enable 
  //the interrupts
  //enable_ints();
  asm volatile("sti");
  while(1);
*/

/*	Paging
  init_paging();
  //Test mapping
  //page_map( get_page( 0x900000, 1, kernel_page_dir), 0, 1, 0x900000);

  //Create a page fault for testing
  char *ptr = (char*)0x900000;
  char val = *ptr;
*/

/*
unsigned char g_320x200x256[] =
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
  for(i = 0; i < 5; i++)
    {
      portb_write(0x3C4, i);
      portb_write(0x3C5, *regs);
      regs++;
    }
  // unlock CRTC registers
  portb_write(0x3C4, 0x03);
  portb_write(0x3D5, portb_read(0x3D5) | 0x80);
  portb_write(0x3D4, 0x11);
  portb_write(0x3D5, portb_read(0x3D5) & ~0x80);

  //regs[0x03] |= 0x80;
  //regs[0x11] &= ~0x80;
  // write CRTC regs
  for(i = 0; i < 25; i++)
    {
      portb_write(0x3D4, i);
      portb_write(0x3D5, *regs);
      regs++;
    }
  // write GRAPHICS CONTROLLER regs 
  for(i = 0; i < 9; i++)
    {
      portb_write(0x3CE, i);
      portb_write(0x3CF, *regs);
      regs++;
    }
  // write ATTRIBUTE CONTROLLER regs 
  for(i = 0; i < 21; i++)
    {
      (void)portb_read(0x3DA);
      portb_write(0x3C0, i);
      portb_write(0x3C0, *regs);
      regs++;
    }
  // lock 16-color palette and unblank display 
  (void)portb_read(0x3DA);
  portb_write(0x3C0, 0x20);
}

#define video_start_mem 0xA0000
#define video_end_mem   0xA0000 + 320 * 200
*/


