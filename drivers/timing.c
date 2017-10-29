#include "timing.h"

#define I8253_CH0   0x40
#define I8253_CMD   0x43
#define I8253_CMD_LOAD 0x34
#define I8253_CMD_LATCH 0x04

#define I8253_FREQ 1193181
#define COUNT_10MS   ( ( I8253_FREQ + 99 ) / 100 )

void init_t(udword frequency){
   udword divisor = I8253_FREQ / frequency;
   ubyte lowByte = (ubyte)(divisor & 0xFF);
   ubyte highByte = (ubyte)( (divisor >> 8) & 0xFF);

   //Initilize the timer
   portb_write( I8253_CMD, 0x36 );
   portb_write( I8253_CH0, lowByte);
   portb_write( I8253_CH0, highByte);
}


void i8253_set( unsigned int c ){

   unsigned char counter;

   portb_write( I8253_CMD, I8253_CMD_LOAD );

   counter = (unsigned char) ( c & 0x00ff );
   portb_write( I8253_CH0, counter );

   counter = (unsigned char) ( ( c >> 8 ) & 0x00ff );
   portb_write( I8253_CH0, counter);
}

unsigned int i8253_get(){

   unsigned int lower_byte, higher_byte;

   portb_write( I8253_CMD, I8253_CMD_LATCH );
   lower_byte = portb_read( I8253_CH0 );
   higher_byte = portb_read( I8253_CH0 );

   return lower_byte + (higher_byte << 8);
}

void delay_second(double sec){

   double num_loops = sec * 100;
   int i = 0;

   while( i <= num_loops){
      i8253_set( COUNT_10MS );
      while( i8253_get() != COUNT_10MS )
	   ;
      i++;

   }
}

