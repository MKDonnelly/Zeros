#include <kernel/sched.h>

//Information about the current thread.
//current_thread points the the thread descriptor
//for the running thread. current_thread_index is
//the index of current_thread in the ALL thread_list.
//cur_context is saved by the interrupt code and passed
//to us.
kthread_t *current_thread = NULL;
int current_thread_index = 0;
thread_context_t *cur_context = NULL;

//thread_list is an ALL (abstract linked list) which is
//used to hold every thread running on the system. 
//we have a total of thread_count - 1 threads in the
//list.
kthread_t *thread_list = NULL;
int thread_count = 0;

//Insert a new thread at the end of the thread list.
void add_thread(kthread_t *thread){
   add_node_ll( (void**)&thread_list, thread, thread_count++);
}

//Called by a thread to voluntarily
//give up processor time. 
//TODO move this to arch since we have assembly
void thread_yield(){
   asm volatile("int $50");
}

//Called by a thread when returning
//Explicitly calling this will give the return
//value. If this is implicitly called when the
//thread index (when k_create_thread sets it up)
//the return value will be undefined.
void thread_exit(void *retval){
   current_thread->state = THREAD_EXIT;
   current_thread->return_value = retval;
   thread_yield();
}

int thread_join_helper(void *element, void *ref){
   kthread_t *current_thread = (kthread_t*)element;
   kthread_t *reference_thread = (kthread_t*)ref;

   if( current_thread->thread_id == reference_thread->thread_id && current_thread->state == THREAD_EXIT ){
      return 1; //Return this kthread_t 
   }else{
      return 0; //Wrong thread
   }
}

void *thread_join( kthread_t *thread_descriptor ){

   kthread_t *thread_to_join;

   do{
      thread_to_join = (kthread_t*)find_node_ll( (void**)&thread_list, thread_descriptor, thread_join_helper );
   }while( thread_to_join == 0 );

   return thread_to_join->return_value;
}

//The default thread to run when
//there is nothing else
static void idle_thread(){
   enable_ints();
   while(1){
      asm volatile("hlt");
   }
}

void init_threading(){

   add_node_ll( (void**)&thread_list, k_create_thread(idle_thread,NULL,NULL,1024), 0 );
   thread_count++;
   current_thread = get_node_ll( (void**)&thread_list, 0 );

   register_interrupt( 50, schedule );
   jump_to_thread( current_thread->context );
}

//*Very* simple scheduler. Runs down the list
//of threads in a round-robin fashion. 
void schedule(){

   //Save the current context
   current_thread->context = cur_context;

   do{
      //Go to the next thread
      current_thread_index = (current_thread_index + 1) % thread_count;
   
      //Set the new current thread and set the new context
      current_thread = get_node_ll( (void**)&thread_list, current_thread_index );
   }while( current_thread->state == THREAD_EXIT );

   cur_context = current_thread->context;
}
