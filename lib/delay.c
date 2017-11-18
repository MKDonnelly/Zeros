
#include "delay.h"

//Use the system clock to implement a delay
void delay(int seconds){
   int curHours = system_time.hours;
   int curSeconds = system_time.seconds;
   int curMilliseconds = system_time.milliseconds;
   
   int endMilliseconds = curMilliseconds;
   int endSeconds = curSeconds + seconds;
   int endHours = curHours;
   while( endSeconds >= 60){
      endHours++;
      endSeconds--;
   }
   
   //This is the waiting part
   while( system_time.milliseconds < endMilliseconds &&
          system_time.seconds      < endSeconds      &&
          system_time.hours        < endHours );
}

