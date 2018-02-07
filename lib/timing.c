#include <lib/timing.h>

system_time_t global_time = {0};

system_time_t timing_get_time(){
   return global_time;
}

//TODO use an ALL to make this neat
//TODO have a oneshot time for delay
void (*callback)() = 0;
int callback_period = 0;
int time_left = 0;

void timing_set_alarm(void (*alarm_function)(), int ms_period){
   callback = alarm_function;
   callback_period = time_left = ms_period;
}

//Increment the global time and call any
//alarms.
void timing_main_handler(){
   global_time.seconds++;
   
   time_left -= 10;
   if( time_left <= 0 ){
      time_left = callback_period;
      callback();
   }
}
