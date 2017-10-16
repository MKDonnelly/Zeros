//Sending this bit to port 0x70 disables
//non-maskable interrupts
#define NMI_disable_bit 0x80
#define CMOS_COMMAND_PORT 0x70
#define CMOS_READ_PORT  0x71

#define CMOS_SECOND_REG   0x00
#define CMOS_MINUTE_REG   0x02
#define CMOS_HOURS_REG    0x04
#define CMOS_WEEKDAY_REG  0x06
#define CMOS_DOM_REG      0x07
#define CMOS_MONTH_REG    0x08
#define CMOS_YEAR_REG     0x09

#include "portio.h"

unsigned short getCMOSReg(unsigned short reg){
   port_byte_out(CMOS_COMMAND_PORT, NMI_disable_bit | reg );
   return port_byte_in(CMOS_READ_PORT); 
}

//TODO: THIS IS BROKEN
void CMOS_delay(int sec){
   unsigned short curSec = getCMOSReg( CMOS_SECOND_REG );
   while( getCMOSReg( CMOS_SECOND_REG ) != curSec + sec);
}

