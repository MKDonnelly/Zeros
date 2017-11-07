
#pragma once

#include "portio.h"
#include "../cpu/isr.h"
#include "../drivers/vgatext/vgatext.h"

#define I8253_CH0   0x40
#define I8253_CMD   0x43
#define I8253_CMD_LOAD 0x34
#define I8253_CMD_LATCH 0x04
#define TIMER_PIC_MASK 0x0E

#define I8253_FREQ 1193181
#define COUNT_10MS   ( ( I8253_FREQ + 99 ) / 100 )

//Set the i8253 to a certain value
//void i8253_set( unsigned int c );
//Get the current counter
//unsigned int i8253_get();
//Using the i8253, delay for a certain
//number of seconds.
//void delay_second(double sec);

//void init_t(udword);


void set_i8253_freq(udword);
void i8253_int_callback(struct registers);
void set_i8253_interrupt();