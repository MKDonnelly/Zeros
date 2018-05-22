#include <arch/x86/pmode/schedule.h>

#include <lib/types.h>
#include <arch/x86/cpu.h>
#include <arch/x86/pmode/isr.h>
#include <arch/x86/pmode/gdt.h>
#include <arch/x86/pmode/pagingasm.h>

//High-level kernel scheduling function
static arch_task_t *current_task = NULL;

//In int.asm. Used to set and get current context
extern context_t *get_current_context();
extern void set_current_context(context_t*);

//Called during isr to save current context into the
//given arch_task_t
void arch_save_context(){
   if( current_task != NULL ){
      current_task->task_context = get_current_context();
   }
}

//Called when an interrupt is returning to load the 
//next task to run
void arch_set_context(){
   set_current_context(current_task->task_context);
}

//Higher level code calls this to set the next task to run
void arch_run_next(arch_task_t *next_task){
   current_task = next_task;

   load_pd( (void*)VIRT_TO_PHYS(next_task->task_pd) );

   //Kernel tasks will have a interrupt_stack of NULL
   //Make sure we do not load that.
   if( next_task->interrupt_stack != NULL )
      set_kernel_stack( (uint32_t)next_task->interrupt_stack );
}

//Explicitly calls scheduler (like with yield)
void arch_trigger_scheduler(){
   arch_trigger_interrupt( ARCH_SCHED_INT );
}

void arch_register_scheduler(void (*sched_handler)()){
   arch_register_interrupt( ARCH_SCHED_INT, sched_handler );
}
