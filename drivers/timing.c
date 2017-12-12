#include "timing.h"



void init_timer(uint8_t enable, int x, int y){

   //Set the received information in system_timer
   system_timer.enablePrint = enable;
   system_timer.x = x;
   system_timer.y = y;

   //Set the timer to operate at 100HZ
   set_timer_freq( I8253_10MS_COUNTER );

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

void set_timer_freq(int freq){
   uint32_t opFreq = freq;

   //Tell the timer we are going to initilize it
   portb_write( I8253_CTRL_P, I8253_LOAD_C );

   //Write the frequency to the timer
   int8_t lowerByte = (uint8_t)(opFreq & 0xFF );
   int8_t higherByte = (uint8_t)( (opFreq >> 8) & 0xFF );
   portb_write( I8253_CH0_P, lowerByte );
   portb_write( I8253_CH0_P, higherByte );
}


//This timer callback handles the system clock
void timer_int_handler( registers_t r){

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
      char hours[9];
      char minutes[9];
      char seconds[9];

      itoa( system_time.hours, hours);
      itoa( system_time.minutes, minutes);
      itoa( system_time.seconds, seconds);

      int clockx = system_timer.x;
      int clocky = system_timer.y;

      k_print_at( "System uptime: ", clockx, clocky);
      k_print_at( hours, clockx+1, clocky+1);
      k_print_at( ":", clockx+2, clocky+1);
      k_print_at( minutes, clockx+4, clocky+1);
      k_print_at( ":", clockx+5, clocky+1);
      k_print_at( seconds, clockx+7, clocky+1);
   }
}
