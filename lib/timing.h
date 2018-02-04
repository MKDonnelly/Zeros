#pragma once

typedef struct{
   int milliseconds;
   int seconds;
   int minutes;
   int hours;
   int days;
   int months;
   int years;
}system_time_t;

system_time_t timing_get_time();
void timing_set_alarm( void (*alarm_function)(), int ms_period);
void timing_main_handler();
