#pragma once

/*    Example usage
   workqueue_t *kwq = workqueue_create();
   tasklet_t *first = tasklet_create( thread1, NULL );
   tasklet_t *second = tasklet_create( thread2, NULL );

   workqueue_add(kwq, first);
   workqueue_add(kwq, second);
 
   workqueue_worker_spawn( kwq );
   workqueue_worker_spawn( kwq );
*/

#include <arch/current_arch>

typedef struct tasklet{
   struct tasklet *next;
   void (*function)();
   void *data;
}tasklet_t;

//Creates a tasklet given a function and data
tasklet_t *tasklet_create( void (*function)(), void *data);

typedef struct{
   unsigned int total_tasks;

   //Linked list of tasklets
   tasklet_t *tasks;
   mutex_t workqueue_lock;   
}workqueue_t;

//Creates a workqueue to use
workqueue_t *workqueue_create();

//Adds a tasklet to the given workqueue
void workqueue_add(workqueue_t *workqueue, tasklet_t *new_task);

//Grabs the next tasklet to run from the workqueue
tasklet_t *workqueue_get(workqueue_t *workqueue);

//Worker thread for a workqueue
void workqueue_worker(workqueue_t *workqueue);

//Spawn a workqueue_worker to work on the given workqueue
void workqueue_worker_spawn(workqueue_t *workqueue);
