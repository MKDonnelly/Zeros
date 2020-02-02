#include <include/lib/keyboard.h>
#include <kernel/task.h>
#include <kernel/sched/sched.h>
#include <lib/print.h>

//TODO turn this into a darray
#define KEYBOARD_BUFFER_MAX 128
static char keyboard_buffer[KEYBOARD_BUFFER_MAX];
static int keyboard_number_chars = 0;

//When a userland process requests a read, we need
//to block the thread and move onto another one.
//This holds the waiting requests
typedef struct urequest{
   int total;
   int bufcount;
   char *buf;
   ktask_t *blocked_process;
}urequest_t;

//TODO turn this into a queue
static urequest_t request = {0, 0, NULL, NULL};

static void keyboard_process_request(){
   if( request.buf != NULL ){
      if( keyboard_number_chars >= request.total ){
         request.buf[request.bufcount++] = keyboard_buffer[--keyboard_number_chars];
         if( request.total == request.bufcount ){
            memset(&request, sizeof(urequest_t), 0);
            request.blocked_process->state = TASK_READY;
         }
      }
   }
}

void keyboard_main_handler(char key_entered){

   //We have space in the buffer.
   if( keyboard_number_chars < KEYBOARD_BUFFER_MAX ){
      keyboard_buffer[keyboard_number_chars++] = key_entered;
   }else{
      //We do not have space in the buffer. Move everything down
      //by one space and get rid of the oldest key.
      for(int i = 0; i < (KEYBOARD_BUFFER_MAX-1); i++){
         keyboard_buffer[i] = keyboard_buffer[i+1];
      }
      keyboard_buffer[keyboard_number_chars] = key_entered;
   }
   k_printf("%c", key_entered);
//   keyboard_process_request();
}

//ONLY called by userland threads
void keyboard_request(int total, char *buf){

   if( keyboard_number_chars < total ){
      request.total = total;
      request.buf = buf;
      request.bufcount = 0;
      request.blocked_process = current_scheduler->scheduler_current_ktask();
      request.blocked_process->state = TASK_BLOCKED;
      current_scheduler->scheduler_yield_task();
   }else{
      k_printf("Returning from keyboard_request\n");
   }
}
