
#include "cmos.h"

//The output of this is a BCD number, which
//needs to be converted to a binary number to
//be of use.

void initCMOS(){
   //We will access bit 2 in status register 2 to output
   //a binary representation of the dates instead of a BCD
   //value which would need to be converted. First, read in
   //the current value, set the bit, and write the value back. 
   portb_write( CMOS_COMMAND_PORT, NMI_disable_bit | 0xB ); 
   unsigned short statusB = portb_read( CMOS_IO_PORT );

   //statusB = statusB | 0x4;   //Set bit 2 in the status register.
   //Now write the status back to the port.
   portb_write( CMOS_COMMAND_PORT, NMI_disable_bit | 0xB );
   portb_write( CMOS_IO_PORT, statusB | 0x4 );
}

unsigned short getCMOSReg(unsigned short reg){
   //Read a value from the cmos. See the header file
   //for definitions of registers.

   portb_write(CMOS_COMMAND_PORT, NMI_disable_bit | reg );
   return portb_read(CMOS_IO_PORT); 
}

//Delay *roughly* "sec" seconds using the CMOS. Roughly since
//the time can be +- 1 second.
void CMOS_delay(int sec){
   int curSec = getCMOSReg( CMOS_SECOND_REG );
   int curMin = getCMOSReg( CMOS_MINUTE_REG );
   
   int endSec = curSec + sec;
   int endMin = curMin; 
   if( endSec >= 60 ){
      endMin++;
      endSec -= 60;
   }

   while( getCMOSReg( CMOS_SECOND_REG ) != endSec && 
	  getCMOSReg( CMOS_MINUTE_REG ) != endMin ) ;
}

