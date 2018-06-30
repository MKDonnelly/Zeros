#pragma once
#include <lib/genericll.h>

typedef struct{
   int milliseconds;
   int seconds;
   int minutes;
   int hours;
   int days;
   int months;
   int years;
}system_time_t;

//Represents a kernel task to
//run at a given interval. This is
//the node definition and is used with
//the abstract linked list.
typedef struct alarm{
   list_head_t timer_list; //Link for use in double linked list

   //The function to call every...
   void (*callback)();

   //this many milliseconds.
   int callback_period;

   //current countdown
   int time_left;
}alarm_t;

//Calls alarm_function every ms_period milliseconds
void timing_set_alarm( void (*alarm_function)(), int ms_period);

//Handles periodic interrupts from arch code
void timing_main_handler();
