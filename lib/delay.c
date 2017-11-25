
#include "delay.h"

//Use the system clock to implement a delay
void delay(int seconds){

   int curMinutes = system_time.minutes;
   int curSeconds = system_time.seconds;
   int curMilliseconds = system_time.milliseconds;
   
   int endMinutes = curMinutes;
   int endSeconds = curSeconds + seconds;
   int endMilliseconds = curMilliseconds;

   while( endSeconds >= 60){
      endMinutes++;
      endSeconds -= 60;
   }
   
   //This is the waiting part
   while( (system_time.seconds <= endSeconds) || 
          (system_time.milliseconds <= endMilliseconds) ) ;
}

