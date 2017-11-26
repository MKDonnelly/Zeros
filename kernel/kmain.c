asm("jmp kmain"); //The bootsector immedietelly jumps to the
                  //start of this file and begins executing. This is
		  //undesirable if there is a function above kmain.
		  //add this so that kmain is always called.

#include "../drivers/portio.h"
#include "../drivers/timing.h"
#include "../drivers/serial/serial.h"
#include "../drivers/cmos.h"
#include "../drivers/pic.h"
#include "../drivers/vgatext/vgatext.h"
#include "../drivers/vga13h/vga13hmode.h"
#include "../drivers/keyboard.h"

#include "../lib/string.h"
#include "../lib/bcd.h"
#include "../lib/types.h"
#include "../lib/bitwise.h"
#include "../lib/delay.h"

#include "../cpu/cpu.h"
#include "../cpu/isr.h"
#include "../cpu/idt.h"
#include "../cpu/gdt.h"

#include "kmalloc.h"
#include "../cpu/paging.h"


//TODO add in struct multiboot* to 
//get information about the system
void kmain(){

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
  //k_print("Enter some text: ");

  //Experimental kmalloc
  heap_init();

  struct heapNode *heapHead = (struct heapNode*)kernel_start_heap;

  //print out debug info
  k_print("Beginning:");
  k_newline();
  char b[20];
  itoh( (int)heapHead->nextChunk, b );
  k_print( b );
  k_newline();
  itoa( (int)heapHead->size, b );
  k_print( b );
  k_newline();
  itoa( (int)heapHead->isAllocated, b );
  k_print( b );
  k_newline();

  char *mem = (char*)kmalloc( 10, 1, 0);
  kfree( mem );

  //print out debug info
  k_print("End:");
  k_newline();
  itoh( (int)heapHead->nextChunk, b );
  k_print( b );
  k_newline();
  itoa( (int)heapHead->size, b );
  k_print( b );
  k_newline();
  itoa( (int)heapHead->isAllocated, b );
  k_print( b );
  k_newline();





  //Paging
  init_paging();

  //Paging testing
  page_map(get_page( 0x2000000, 1, kernel_page_dir ), KERNEL_MEMORY, IS_WRITEABLE, 0xb8000);
  char *vid = (char*)0x2000000;
  *(vid + 20) = 'z';


  //ALWAYS have this, or else the program
  //will run off the end of the world.
  while(1);
}

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



/*        TODO Switch to 13h mode from protected mode
unsigned char mode13h[] = {
   //General registers
   0x63, 0x0, 0x70, 0x4,
   //Sequencer
   0x3, 0x1, 0xF, 0x0, 0xE,
   //CRTC
   0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F, 0x0, 0x41,
   0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x9c, 0x8e, 0x8f, 0x28,
   0x40, 0x96, 0xB9, 0xA3, 0xFF,
   //Graphics
   0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0x5, 0xF, 0xFF,
   //attribute
   0,1,2,3,4,5,6,7,8,9,0xA,0xB,0xC,0xD,0xE,0xF,0x11,0x12,0x13,0x14,0x15
};

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


