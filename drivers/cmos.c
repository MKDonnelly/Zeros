
#include "cmos.h"

//The output of this is a BCD number, which
//needs to be converted to a binary number to
//be of use.

void initCMOS(){
   //We will access bit 2 in status register 2 to output
   //a binary representation of the dates instead of a BCD
   //value which would need to be converted. First, read in
   //the current value, set the bit, and write the value back. 
   portb_write( CMOS_COMMAND_P, NMI_disable_bit | 0xB ); 
   unsigned short statusB = portb_read( CMOS_IO_P );

   //Now write the status back to the port.
   portb_write( CMOS_COMMAND_P, NMI_disable_bit | 0xB );
   portb_write( CMOS_IO_P, statusB | 0x4 );
}

uint16_t getCMOSReg(uint16_t reg){
   //Read a value from the cmos. See the header file
   //for definitions of registers.
  
   //Block while a CMOS update is in progress	
   while(cmos_update());

   portb_write(CMOS_COMMAND_P, NMI_disable_bit | reg );
   return portb_read(CMOS_IO_P); 
}

int8_t cmos_update(){
   portb_write( CMOS_COMMAND_P, 0x0A);
   return portb_read( CMOS_IO_P & 0x80 );
}
