
#include "delay.h"

//Use the system clock to implement a delay
//We will assume that no dramatically long
//delay will be entered (such as a 1-day delay)
void k_delayms(int milliSeconds){

   int curMilliseconds = system_time.milliseconds;
   int curSeconds = system_time.seconds;
   int curMinutes = system_time.minutes;
   int curHours = system_time.hours;
   
   int endMilliseconds = curMilliseconds + milliSeconds;
   int endSeconds = curSeconds;
   int endMinutes = curMinutes;
   int endHours = curHours;

   //Handle a milli second overflow
   while( endMilliseconds >= 1000 ){
      endMilliseconds -= 1000;
      endSeconds++;
   }

   //Handle a second overflow
   while( endSeconds >= 60){
      endMinutes++;
      endSeconds -= 60;
   }
   //Handle a minute overflow
   while( endMinutes >= 60){
      endHours++;
      endMinutes -= 60;
   }
   
   //This is the waiting part
   while( (system_time.hours < endHours )    ||
          (system_time.minutes < endMinutes) || 
          (system_time.seconds < endSeconds) || 
          (system_time.milliseconds < endMilliseconds) ) ;
}

//Just re-use the kdelay_ms function
void k_delays(int seconds){
   k_delayms( seconds * 1000 );
}
