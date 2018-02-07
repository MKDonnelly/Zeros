#include <kernel/sched.h>

#define MAX_THREADS 20
kthread_t *threads[MAX_THREADS] = {0};

int next_free_thread = 1;
int cur_thread_index = 0;
//The current context. Used as an interface
//with the arch context save to swap in/out
//a thread context.
thread_context_t *cur_context = NULL;

void add_thread(kthread_t *thread){
   if( next_free_thread < MAX_THREADS ){
      threads[next_free_thread] = thread;
      next_free_thread++;
   }
}

//Called by a thread to voluntarily
//give up processor time.
void thread_yield(){
   asm volatile("int $50");
}

//Called by a thread when returning
//Explicitly calling this will give the return
//value. If this is implicitly called when the
//thread index (when k_create_thread sets it up)
//the return value will be undefined.
void thread_exit(void *retval){
   threads[cur_thread_index]->state = THREAD_EXIT;
   threads[cur_thread_index]->return_value = retval;
   thread_yield();
}

void *thread_join( int thread_id ){
   while(1){
      for(int i = 0; i < next_free_thread; i++){
         if( threads[i]->thread_id == thread_id && threads[i]->state == THREAD_EXIT ){
            return threads[i]->return_value;
         }
      }
      thread_yield();
   }
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

   //Setup the idle thread and jump to it
   threads[0] = k_create_thread( idle_thread, NULL, NULL, 1024);
   register_interrupt( 50, schedule);

   jump_to_thread( threads[0]->context );
}

//*Very* simple scheduler. Runs down the list
//of threads in a round-robin fashion. 
void schedule(){
   //Save the current context
   threads[cur_thread_index]->context = cur_context;

   //Go to the next thread 
   cur_thread_index++;
   cur_thread_index %= next_free_thread;
   while( threads[cur_thread_index]->state == THREAD_EXIT ){
      cur_thread_index++;
      cur_thread_index %= next_free_thread;
   }

   //Grab the context from this thread
   cur_context = threads[cur_thread_index]->context;
}

