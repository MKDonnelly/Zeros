
#pragma once

#include "portio.h"
#include "../cpu/isr.h"
#include "../drivers/vgatext/vgatext.h"

#define I8253_CH0_P   0x40 //This can generate an interrupt
//CH1 @ 0x41 is not implemented
#define I8253_CH2_P   0x42 //This controls the PC speaker
#define I8253_CTRL_P  0x43

//Configuration bits for port 0x43:
//(from wiki.osdev.or/Programmable_Interval_Timer)
//Bits           Use
//6-7            Channel select:
//                 0 0 = Channel 0
//                 0 1 = Channel 1
//                 1 0 = Channel 2
//                 1 1 = Read-back command
//4-5            Access mode
//                 0 0 = Latch count value command
//                 0 1 = lowbyte only
//                 1 0 = highbyte only
//                 1 1 = lowbyte/highbyte
//1-3            Timer Mode
//                 0 0 0 = Interrupt on terminal count
//                 0 0 1 = Hardware re-triggerable one-shot
//                         (only works on CH 2)
//                 0 1 0 = Rate generator
//                 0 1 1 = Square wave generator
//                 1 0 0 = Software triggered strobe
//                 1 0 1 = Hardware triggered strobe
//                         (only works on CH 2)
//                 1 1 0 = same as 0 1 0
//                 1 1 1 = same as 0 1 1
//0              Binary (0) or BCD (1) mode

//commands to send to timer
#define I8253_LOAD_C   0x34
#define I8253_LATCH_C  0x04

#define I8253_FREQ 1193182
#define I8253_10MS_COUNTER 11932

//A primitive system uptime clock
struct sys_time{
   int days;
   int hours;
   int minutes;
   int seconds;
   int milliseconds;
} system_time;

//Initilize the timer
//Enable its IRQ and handler
void init_timer();

//Handler function for the timer
void set_timer_freq(int);
void timer_int_handler( struct registers);

//TODO
//set_timer_mode();
//get_timer_counter();
//set_timer_divider();
