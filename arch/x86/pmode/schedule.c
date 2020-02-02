#include <arch/x86/pmode/schedule.h>

#include <lib/types.h>
#include <arch/x86/cpu.h>
#include <arch/x86/pmode/isr.h>
#include <arch/x86/pmode/gdt.h>
#include <arch/x86/pmode/pagingasm.h>
#include <arch/x86/pmode/tss.h>

//High-level kernel scheduling function
arch_task_t *current_task = NULL;

context_t *get_current_context(){
   if( current_task != NULL ){
      //return current_task->callstack.stacks[current_task->callstack.current_stack];
      int stack = current_task->callstack.stacks[0] != NULL ? 0 : 1;
      return current_task->callstack.stacks[stack];
   }
   return NULL;
}

//Called during isr to save current context into the
//given arch_task_t
void arch_save_context(context_t *saved_context){
   if( current_task != NULL ){
      int next_stack = current_task->callstack.stacks[0] != NULL ? 0 : 1;
      current_task->callstack.stacks[next_stack] = saved_context;
   }
}

//Called when an interrupt is returning to load the 
//next task to run.
context_t *arch_set_context(){
   if( current_task != NULL ){
      int current_stack = current_task->callstack.stacks[1] == NULL ? 0 : 1;
      context_t *temp = current_task->callstack.stacks[current_stack];
      current_task->callstack.stacks[current_stack] = NULL;
      return temp;
   }
   return NULL;
}

//Higher level code calls this to set the next task to run
void arch_run_next(arch_task_t *next_task){
   current_task = next_task;

   load_pd( (void*)VIRT_TO_PHYS(next_task->task_pd) );

   //Kernel tasks will have a interrupt_stack of NULL
   //Make sure we do not load that.
   if( next_task->interrupt_stack != NULL )
      tss_set_kstack( (uint32_t)next_task->interrupt_stack );
}

//Explicitly calls scheduler (like with yield)
void arch_trigger_scheduler(){
   arch_trigger_interrupt( ARCH_SCHED_INT );
}

void arch_register_scheduler(void (*sched_handler)()){
   arch_register_interrupt( ARCH_SCHED_INT, sched_handler );
}
