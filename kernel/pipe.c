#include <kernel/pipe.h>
#include <fs/vfs/vnode.h>
#include <arch/x86/pmode/spinlock.h>
#include <kernel/mm/heap.h>

void pipe_create(fs_node_t **pipe_input, fs_node_t **pipe_output){
   //Create new pipe object
   pipe_t *p = k_malloc(sizeof(pipe_t), 0);

   //Initialize spinlock
   spinlock_init(&p->lock);

   //This is a new pipe, so the producer and consumer
   //have not closed the pipe
   p->producer_closed = p->consumer_closed = 0;

   //No data initially
   p->data_len = 0;

   //Create a vfs node for the input and output
   fs_node_t *input = k_malloc(sizeof(fs_node_t), 0);
   strcpy(input->name, "pipe input");
   input->type = FS_PIPE;
   input->fs = NULL; //field not used
   //Inode points to pipe_t object
   input->inode = (void*)p; 
   //Because this is the input pipe, only the write
   //operation is defined
   input->write = pipe_write;
   input->read = NULL;
   input->close = pipe_close;
   //Return input vfs node
   *pipe_input = input;

   //Now create a vfs node for the output 
   fs_node_t *output = k_malloc(sizeof(fs_node_t), 0);
   strcpy(output->name, "pipe output");
   output->type = FS_PIPE;
   output->fs = NULL; //field not used
   //Inode points to pipe_t object
   output->inode = (void*)p; 
   output->read = pipe_read;
   output->write = NULL;
   output->close = pipe_close;
   //Return output vfs node
   *pipe_output = output;
}

void sys_makepipe(int *fdin, int *fdout){
   fs_node_t *pipe_in;
   fs_node_t *pipe_out;
   pipe_create(&pipe_in, &pipe_out);

   //Setup pipe_in and pipe_out to be in open fds for
   //the calling process
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();

   //Find two free descriptors to place fdin and fdout
   for(int i = 0; i < 20; i++){
      if( ctask->open_fs[i] == NULL ){
         *fdin = i;
         ctask->open_fs[i] = pipe_in;
         break;
      }
   }
   for(int i = 0; i < 20; i++){
      if( ctask->open_fs[i] == NULL ){
         *fdout = i;
         ctask->open_fs[i] = pipe_out;
         break;
      }
   }
}

//Returns -1 if the consumer closed
int pipe_write(fs_node_t *pipe, int offset, int len, char *buf){
   //offset is ignored

   //Get pipe object
   pipe_t *p = (pipe_t*)pipe->inode;

   if( p->consumer_closed ){
      return -1;
   }

   //Wait if there is data already in the pipe
   while( p->data_len > 0 ){
      current_scheduler->scheduler_yield_task();
   }

   //Acquire lock
   spinlock_acquire(&p->lock);

   //Write data to pipe
   memcpy(p->data, buf, len);
   p->data_len = len;
      
   //Free lock
   spinlock_release(&p->lock);
   return 0;
}

//Read data from the pipe into the buffer buf up to a maximum of len
//bytes.  Offset is ignored.  Returns -1 if the producer closed the
//pipe and there is nothing remaining to read, 0 otherwise.
int pipe_read(fs_node_t *pipe, int offset, int len, char *buf){

   //Get pipe object
   pipe_t *p = (pipe_t*)pipe->inode;

   if( p->producer_closed && p->data_len == 0 ){
      return 0;
   }

   //Check if there is any data to read
   while( p->data_len == 0 && !p->producer_closed){
      current_scheduler->scheduler_yield_task();
   }

   //Acquire lock
   spinlock_acquire(&p->lock);

   memcpy(buf, p->data, p->data_len);
   int temp = p->data_len;
   p->data_len = 0;
  
   //free lock
   spinlock_release(&p->lock);
   return temp;
}

int pipe_close(fs_node_t *pipe){
   //Get reference to pipe_t object
   pipe_t *p = (pipe_t*)pipe->inode;

   //The procedure for closing the pipe is 
   //different depending on if we are the
   //producer or consumer
   if( pipe->read == NULL ){
      //We are closing the producer (writing) end
      if( p->consumer_closed ){
         //consumer already closed so we can free pipe_t
         k_free(p);
         k_free(pipe);
         return 0;
      }else{
         //consumer is still active.  Indicate that the
         //producer is closing the pipe and free the vfs node
         p->producer_closed = 1;
         k_free(pipe);
         return -1;
      }
   }else{ //pipe->write == NULL
      //we are closing the consumer side
      if( p->producer_closed ){
         //producer already closed so we can free pipe_t
         k_free(p);
         k_free(pipe);
         return 0;
      }else{
         //producer is still active so indicate the consumer is
         //closing the pipe and free the vfs node.
         p->consumer_closed = 1;
         k_free(pipe);
         return -1;
      }
   }
}
