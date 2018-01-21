#include <sched.h>

#define MAX_THREADS 20
kthread_t *threads[MAX_THREADS] = {0};

int next_free_thread = 1;
int cur_thread_index = 0;
thread_context_t *cur_context = NULL;

void add_thread(kthread_t *thread){
   if( next_free_thread < MAX_THREADS ){
      threads[next_free_thread] = thread;
      next_free_thread++;
   }
}

void thread_yield(){
   asm volatile("int $50");
}

static void idle_thread(){
   enable_ints();
   while(1){
      asm volatile("hlt");
   }
}

void init_threading(){

   //Setup the idle thread and jump to it
   threads[0] = k_create_thread( idle_thread, NULL, NULL, 1024, 0);
   register_interrupt( 50, schedule);

   jump_to_thread( threads[0]->context );
}

//*Very* simple scheduler. Runs down the list
//of threads in a round-robin fashion. 
void schedule(){

   //Save the current context
   threads[cur_thread_index]->context = cur_context;

   //Go to the next thread index
   cur_thread_index++;

   //cur_thread_index %= next_free_thread;
   if( cur_thread_index == next_free_thread )
      cur_thread_index = 0;

   //Grab the context from this thread
   cur_context = threads[cur_thread_index]->context;
}

