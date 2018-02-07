#pragma once

#include <kernel/kmalloc.h>
#include <lib/abstract_ll.h>

typedef struct{
   int milliseconds;
   int seconds;
   int minutes;
   int hours;
   int days;
   int months;
   int years;
}system_time_t;

typedef struct alarm{
   struct alarm *next;

   //Run when time_remaining <= 0
   void (*callback)();

   //The interval at which to
   //call callback
   int callback_ms_period;

   //Current time until triggering
   //callback
   int time_remaining;

}alarm_t;

void timing_set_alarm( void (*alarm_function)(), int ms_period);
void timing_main_handler();
system_time_t timing_get_time();
