#include <arch/x86/pmode/schedule.h>

//High-level kernel scheduling function
static void (*sched_callback)() = NULL;
static arch_task_info_t *current_task = NULL;

//To be removed
extern context_t *cur_context;

void arch_run_next(arch_task_info_t *next_task){
   current_task = next_task;

   load_pd( (void*)VIRT_TO_PHYS(next_task->task_pd) );

   //Kernel tasks will have a interrupt_stack of NULL
   //Make sure we do not load that.
   if( next_task->interrupt_stack != NULL )
      set_kernel_stack( (uint32_t)next_task->interrupt_stack );
}


void setup_sched(void (*sched_call_back)(), uint32_t ms_period){
   arch_register_interrupt( ARCH_SCHED_INT, handle_sched_int );
   sched_callback = sched_call_back;
   timing_set_alarm( handle_sched_int, ms_period );
}

void handle_sched_int(registers_t *state){

   //This is one of the first pieces of code to run in
   //a scheduler interrupt. Our first job is to save the
   //state of the interrupted task
   if( current_task != NULL ){
      current_task->task_context = cur_context;
   }

   //Call high-level scheduler function to decide which
   //task to run next (set by arch_run_next)
   sched_callback();

   //We cannot load cur_context in arch_run_next since arch_run_next
   //may be called right in the middle of scheduler code, which would
   //mess everything up. We must call it just as we are leaving the
   //scheduler interrupt (i.e. here)
   cur_context = current_task->task_context;
}
