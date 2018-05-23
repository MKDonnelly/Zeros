#include <lib/generic_ll.h>

#include <kernel/mm/heap.h>

//Internally used and NEVER used by outside code.
//This is meant to cast an unknown list element into
//something that we can work with. 
typedef struct mem{
   struct mem *next;
}mem_t;

void __list_create( void **list, int count, int element_size){
   mem_t **head = (mem_t**)list;

   if( count == 0 ){
      *head = NULL;
   }else{
      *head = (mem_t*)k_malloc(element_size, 0);
      __list_create( (void**)&((*head)->next), count - 1, element_size );
   }
}


void __list_dealloc(void **list){
   mem_t **head = (mem_t**)list;
   if( *head == NULL ){
      return;
   }else{
      __list_dealloc( (void**)&(*head)->next );
      k_free( *head );
      *head = NULL;
   }   
}


void __list_add( void **list, void *element, int index){
   mem_t **head = (mem_t**)list;

   //By default, if the index lies beyond the end of the list, 
   //we append it to the end.
   if( *head == NULL ){
      *head = element;
      (*head)->next = NULL;
   }else if( index == 0 ){
      //Replace the head node
      mem_t *temp = *head;
      (*head) = element;

      mem_t *new_head = (mem_t*)element;
      new_head->next = temp;
   }else{
      __list_add( (void**)&(*head)->next, element, index - 1 );
   }
}


//Returns the node given at index and removes
//it from the list. Returns null on error.
void *__list_rm_index(void **list, int index){
   mem_t **head = (mem_t**)list;
   //Check for an empty list
   if( *head == NULL ){
      return NULL;
   }else if( index == 0 ){
      mem_t *ret = *head;
      *head = (*head)->next;
      return ret;
   }else{
      __list_rm_index( (void**)&(*head)->next, index - 1 );
   }
   return NULL;
}

//Removes the node given a pointer to it
void __list_rm_node(void **list, void *element){
   mem_t **head = (mem_t**)list;
   if( (void*)*head == element ){
      *head = (*head)->next;
   }else{
      __list_rm_node( (void**)&(*head)->next, element );
   }
}


//Returns the node given at index but does
//not remove the node. Returns null on error
void *__list_get_node(void **list, int index){
   mem_t **head = (mem_t**)list;
   //NULL is returned if the index lies outside of the list
   if( *head == NULL ){
      return NULL;
   }else if( index == 0 ){
      return (void*)*head;
   }else{
      return (void*)__list_get_node((void**)&(*head)->next, index - 1 );
   }
}

//Replaces the node at index with new_node and
//returns the old node at that index. Returns null on error.
void *__list_replaceby_index(void **list, void *new_node, int index){
   mem_t **head = (mem_t**)list;

   //Return null if the index lies beyond the end of the list
   if( *head == NULL && index != 0 ){
      return NULL;
   }else if( index == 0 ){
      mem_t *ret = *head;
      *head = new_node;
      return ret;
   }else{
      return (void*)__list_replaceby_index( (void**)&(*head)->next, 
                                        new_node, index - 1 );
   }
}

void *__list_replaceby_node(void **list, void *new_node, void *old){
   mem_t **head = (mem_t**)list;
   if( (void*)*head == old ){
      mem_t *ret = *head;
      *head = new_node;
      return ret;
   }else{
      return (void*)__list_replaceby_node( (void**)&(*head)->next, 
                                        new_node, old );
   }
}

int __list_size( void **list ){
   mem_t **head = (mem_t**)list;
   if( *head == NULL ){
      return 0;
   }else{
      return 1 + __list_size( (void**)&(*head)->next );
   }
}
