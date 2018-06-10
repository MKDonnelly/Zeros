#pragma once

typedef struct{
   unsigned int milliseconds;
   unsigned int seconds;
   unsigned int minutes;
   unsigned int hours;
   unsigned int days;
   unsigned int months;
   unsigned int years;
}system_time_t;

//Represents a kernel task to
//run at a given interval. This is
//the node definition and is used with
//the abstract linked list.
typedef struct alarm{
   struct alarm *next;

   //The function to call every...
   void (*callback)();

   //this many milliseconds.
   unsigned int callback_period;

   //current countdown
   int time_left;
}alarm_t;

//Calls alarm_function every ms_period milliseconds
void timing_set_alarm( void (*alarm_function)(), unsigned int ms_period);

//Handles periodic interrupts from arch code
void timing_main_handler();
