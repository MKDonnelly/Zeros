#include <lib/timing.h>

//Holds the system time
system_time_t global_time = {0};

alarm_t *alarm_list = NULL;
int total_alarms = 0;

void timing_set_alarm(void (*alarm_function)(), int ms_period){

   alarm_t *new_alarm = (alarm_t*)k_malloc( sizeof(alarm_t), 0);
   new_alarm->callback = alarm_function;
   new_alarm->callback_period = new_alarm->time_left = ms_period;

   add_node_ll( alarm_list, new_alarm, 0);
   total_alarms++;
}


//Increment the global time and call any
//alarms.
void timing_main_handler(){
   global_time.seconds++;

   if( total_alarms ){
      //This runs down the list of alarms and handles each
      foreach_in_gll( alarm_list, alarm_iter ){
         alarm_iter->time_left -= ARCH_TIMER_MS_PERIOD;
         
         //Handle any alarms that go off
         if( alarm_iter->time_left <= 0 ){
            alarm_iter->time_left = alarm_iter->callback_period;
            alarm_iter->callback();
         }
      }      
   }
}
