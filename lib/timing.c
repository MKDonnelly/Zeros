#include <lib/timing.h>

#include <kernel/mm/heap.h>
#include <arch/x86/archx86.h>
#include <lib/types.h>

//Holds the system time
static system_time_t global_time = {0};

static list_t alarm_list = LIST_SINIT(__builtin_offsetof(alarm_t, timer_list));

void timing_set_alarm(void (*alarm_function)(), int ms_period){

   alarm_t *new_alarm = (alarm_t*)k_malloc( sizeof(alarm_t), 0);
   new_alarm->callback = alarm_function;
   new_alarm->callback_period = new_alarm->time_left = ms_period;

   list_pushfront(&alarm_list, new_alarm);
}


//Increment the global time and call any
//alarms.
void timing_main_handler(){
   //TODO: actually, each time this is called,
   //      10ms should have elapsed.
   global_time.seconds++;

   if( alarm_list.len > 0 ){
      //This runs down the list of alarms and handles each
      list_foreach( &alarm_list, alarm_iter, alarm_t ){
         alarm_iter->time_left -= ARCH_TIMER_MS_PERIOD;
         
         //Handle any alarms that go off
         if( alarm_iter->time_left <= 0 ){
            alarm_iter->time_left = alarm_iter->callback_period;
            alarm_iter->callback();
         }
      }
   }
}
