#include <lib/timing.h>

//Holds the system time
system_time_t global_time = {0};

alarm_t *alarm_list = NULL;
int total_alarms = 0;

void timing_set_alarm(void (*alarm_function)(), int ms_period){

   alarm_t *new_alarm = (alarm_t*)kmalloc( sizeof(alarm_t), 0, 0 );
   new_alarm->callback = alarm_function;
   new_alarm->callback_period = new_alarm->time_left = ms_period;

   add_node_ll( (void**)&alarm_list, new_alarm, 0);
   total_alarms++;
}

//used with find_node_ll to run down
//the abstract linked list of alarms.
//decrements the time remaining and handles
//any alarms that go off
int handle_alarms(void *alarm1, void *alarm2){

   //alarm1 will take on each item in the alarm list
   alarm_t *current_alarm = (alarm_t*)alarm1;

   //Subtract off 10ms. We assume that is the
   //delay between successive timer interrupts.
   current_alarm->time_left -= ARCH_TIMER_MS_PERIOD;

   //Handle alarms that go off
   if( current_alarm->time_left <= 0 ){
      current_alarm->time_left = current_alarm->callback_period;
      current_alarm->callback();
   }

   //Always return 0 so that we run down the linked list
   return 0;
}

//Increment the global time and call any
//alarms.
void timing_main_handler(){
   global_time.seconds++;

   if( total_alarms ){
      //This runs down the list of alarms and handles each
      find_node_ll( (void**)&alarm_list, NULL, handle_alarms );
   }
}
