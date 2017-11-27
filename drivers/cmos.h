
#pragma once

#include <types.h>
#include <portio.h>

//Sending this bit to port 0x70 disables
//non-maskable interrupts
#define NMI_disable_bit 0x80

#define CMOS_COMMAND_P 0x70
#define CMOS_IO_P  0x71

#define CMOS_SECOND_REG   0x00
#define CMOS_MINUTE_REG   0x02
#define CMOS_HOUR_REG    0x04
#define CMOS_WEEKDAY_REG  0x06
#define CMOS_DOM_REG      0x07
#define CMOS_MONTH_REG    0x08
#define CMOS_YEAR_REG     0x09

//Initilize the cmos
void initCMOS();

//Get the designated cmos register 
//(see the #define's above)
unsigned short getCMOSReg(unsigned short);

//We need to check to make sure a CMOS
//time update is not running when we get
//a value
int cmos_update();

//Enable the RTC to generate a periodic
//interrupt on IRQ 8. This can be used
//just like the timer, but with less 
//precision.

