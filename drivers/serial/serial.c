#include "serial.h"

void sp_init(){
   portb_write( COM1_IOPORT + 1, 0x00 ); //Disable interrupts for serial port
   //Set the baud rate
   portb_write( COM1_IOPORT + 3, 0x80); //Set the MSB of the LCR
   portb_write( COM1_IOPORT + 0, 0x03);     //Set divisor to 3 (38400 baud)
   portb_write( COM1_IOPORT + 1, 0x00);     //     set upper bit
   portb_write( COM1_IOPORT + 3, 0x03); //User Line Control Register to set
                                        //8-bit, no parity, one stop bit
   portb_write( COM1_IOPORT + 2, 0xC7); //Enable FIFO bugger, clear them w/
                                          //14-byte threshold
}

int sp_transmit_empty(){
   return portb_read(COM1_IOPORT + 5) & 0x20;
}

void sp_putchar(char c){
   while( sp_transmit_empty() == 0);  //Wait for everything to transmit first
   portb_write( COM1_IOPORT, c);      //Send the character
}

void sp_putstr(char *string){
   int i = 0;
   while( string[i] != '\0' ){
      sp_putchar(string[i]);
      i++;
   }
}
