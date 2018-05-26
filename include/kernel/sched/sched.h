#pragma once

//Generic interface to scheduler routines
typedef struct{
   //Does any pre-initilization for the scheduler
   //After calling this, the scheduler should be
   //able to accept tasks into a queue. This could
   //possible take a paramater like how often to make
   //a task switch.
   void (*scheduler_setup)();

   //This starts the scheduler after setup_scheduler
   //is called. This never returns.
   void (*scheduler_start)();

   //Adds a task to the scheduler
   void (*scheduler_add_task)();

   //Removes a task from the scheduler
   void (*scheduler_rm_task)();

   //Used on blocking calls
   void (*scheduler_yield_task)();

   //Used to cleanly exit a task
   void (*scheduler_exit_task)();

   //Used to join with a given task
   void *(*scheduler_join_task)();

   //Returns the current ktask 
   void *(*scheduler_current_ktask)();

   //This does the actual task switching and is
   //routinely called.
   void (*scheduler_schedule)();

}schedalg_t;

extern schedalg_t *current_scheduler;
