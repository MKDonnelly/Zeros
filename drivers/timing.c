#include "timing.h"


void init_timer(uint8_t enable, int x, int y){

   //Set the received information in system_timer
   system_timer.enablePrint = enable;
   system_timer.x = x;
   system_timer.y = y;

   //Set the timer to operate at 100HZ
   set_timer_count( I8253_10MS_COUNTER );

   //Enable the timer interrupt and 
   //set the interrupt handler
   enable_irq( TIMER_IRQ );
   register_interrupt( TIMER_INTERRUPT, timer_int_handler );

   //Zero out the members of the sys_time struct
   //to be safe
   system_time.days = 0;
   system_time.hours = 0;
   system_time.minutes = 0;
   system_time.seconds = 0;
   system_time.milliseconds = 0;
}

void set_timer_count(uint16_t count){

   //Select Channel 0, set the access mode to
   //both low/high byte, set it to operate in
   //mode 3, and use binary instead of BCD 
   portb_write( I8253_CTRL_P, I8253_CTRL_CH0_M |
                              I8253_CTRL_LHBYTE_M |
                              I8253_CTRL_MODE3_M |
                              I8253_CTRL_BIN_M );

   //Write the frequency to the timer
   int8_t lowerByte = (uint8_t)(count & 0xFF );
   int8_t higherByte = (uint8_t)( (count >> 8) & 0xFF );
   portb_write( I8253_CH0_P, lowerByte );
   portb_write( I8253_CH0_P, higherByte );
}

uint16_t get_timer_count(){
   portb_write( I8253_CTRL_P, 0 );
   
   uint16_t result;
   uint8_t tmp;
   tmp = portb_read( 0x40 );
   result = tmp;
   tmp = portb_read( 0x40 );
   result |= (tmp << 8 );

   return result;
}


//This timer callback handles the system clock
void timer_int_handler( registers_t r){

/*
   //This will be set if the seconds 
   //have overflowed. We can then decide
   //if we should print the system uptime
   //to the screen.
   int8_t secondOverflow = 0;

   //This is hardcoded, so, for now, we will assume
   //that we have set the clock to use .01 second
   //interrupt intervals
   system_time.milliseconds += 10;

   //Miliseconds overflowed
   if( system_time.milliseconds >= 1000 ){
      system_time.milliseconds -= 1000;
      system_time.seconds += 1;
      secondOverflow = 1;
   }
   //Seconds overflowed
   if( system_time.seconds >= 60 ){
      system_time.seconds -= 60;
      system_time.minutes++;
   }
   //Minutes overflowed
   if( system_time.minutes >= 60 ){
      system_time.minutes -= 60;
      system_time.hours++;
   }

   //If hours overflowed
   if( system_time.hours >= 24 ){
      system_time.hours -= 24;
      system_time.days++;
   }

   //Update the clock if needed on the screen
   if( system_timer.enablePrint && secondOverflow ){
      char hours[9] = {0};
      char minutes[9] = {0};
      char seconds[9] = {0};

      itoa( system_time.hours, hours);
      itoa( system_time.minutes, minutes);
      itoa( system_time.seconds, seconds);

      int clockx = system_timer.x;
      int clocky = system_timer.y;

      k_printf_at( "System uptime: ", clockx, clocky);
      k_printf_at( hours, clockx+1, clocky+1);
      k_printf_at( ":", clockx+2, clocky+1);
      //Print two spaces so that the field is clear
      //if we don't, we will print "59" and then
      //print a "0" on top of the five, giving the false
      //impression that we went from 59 to 9 seconds when
      //the seconds overflow.
      k_printf_at( "  ", clockx+4, clocky+1);
      k_printf_at( minutes, clockx+4, clocky+1);
      k_printf_at( ":", clockx+5, clocky+1);
      //Same as above
      k_printf_at( "  ", clockx+7, clocky+1);
      k_printf_at( seconds, clockx+7, clocky+1);
   }
*/
   //Switch tasks
   //schedule();
}
