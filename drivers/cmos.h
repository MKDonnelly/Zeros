
#pragma once

#include "../lib/types.h"
#include "../lib/bcd.h"
#include "portio.h"

//Sending this bit to port 0x70 disables
//non-maskable interrupts
#define NMI_disable_bit 0x80
#define CMOS_COMMAND_PORT 0x70
#define CMOS_IO_PORT  0x71

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
//Using the cmos real time clock, delay for
//a certain number of seconds.
void CMOS_delay(int);

