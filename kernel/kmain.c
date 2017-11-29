#include <portio.h>
#include <timing.h>
#include <serial/serial.h>
#include <cmos.h>
#include <pic.h>
#include <vga3h/vga3h.h>
#include <vga13h/vga13h.h>
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
#include <vgacommon.h>

void kmain(){

  set_vga_mode( vga_3h_mode );
  init_vga(0);

  init_gdt();
  remap_pic();
  init_interrupts();
  init_timer(1, 0, 0);
  init_keyboard();
  enable_ints();

  k_clear_screen();
  k_newline();
  k_newline();
  k_printf("Enter some text: ");

  while(1);
}


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
//VGA 13h mode

  char *vidmem = (char*)0xA0000;
  set_vga_mode( vga_13h_mode );


  for( int i = 0, j = 0; i < 320 * 200; i++, j++){
     vidmem[i] = j;
     if( j >= 255 )
        j = 0;
  }

  for( int k = 0; k < 320 * 2; k++){
     vidmem[k] = 5;
  }
*/



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
