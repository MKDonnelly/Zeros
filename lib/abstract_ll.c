#include <include/lib/abstract_ll.h>

//Internally used by create_ll
//NEVER used by outside code.
typedef struct mem{
   struct mem *next;
}mem_t;

//Creates a linked list. Callers will need to pass in
//create_ll( (void**)&my_list, ...)
void create_ll( void **list, int count, int element_size){

   //Validate arguments. If any are invalid, return. 
   if( list == 0 || count < 0 || element_size <= 0 ){
      return;
   }

   if( count == 0 ){
      *((mem_t**)list) = NULL;
      return;
   }

   mem_t **head = (mem_t**)list;
   (*head) = (mem_t*)k_malloc(kernel_heap, element_size, 0, 0);

   mem_t *next_node = (*head);

   while( --count > 0 ){
      //Allocate the *next pointer and jump through it.
      next_node->next = (mem_t*)k_malloc(kernel_heap, element_size, 0, 0 );
      next_node = next_node->next;
   }

   //Make sure the next pointer of the last node is null
   next_node->next = NULL;
}


void dealloc_ll(void **list){

   mem_t **h = (mem_t**)list;

   mem_t *head = (mem_t*)(*h);
   mem_t *next = (mem_t*)(*h);
   
   while( head != NULL ){
      next = head->next;
      k_free( kernel_heap, head );
      head = next;
   }
}

//Adds element at index in list. Returns -1 on error
int add_node_ll( void **list, void *element, int index){

   //Return if the element is null or the index is bad
   //Also return if we are asked to place an element
   //at index != 0 with the head null
   if( (element == 0 || index < 0) || 
       ( *((mem_t**)list) == NULL && index != 0  ) )
      return -1;

   mem_t **head = (mem_t**)list;

   //Adding an element to the head is a special case
   if( index == 0 ){      
      mem_t *tmp = *head;
      (*head) = element;
      
      mem_t *new_head = (mem_t*)element;
      new_head->next = tmp;
      return 0;
   }
   
   int cur_index = 0;

   //use next to step through the list
   mem_t *stepper = *head;

   //Move to the node just before the index we were given
   while( cur_index != (index-1) && stepper != NULL ){
      stepper = stepper->next;
      cur_index++;
   }

   //Verify that we really are at node index-1
   //Also catch the special case of adding to the end
   if( cur_index == (index-1) && stepper == NULL ){
      stepper = (mem_t*)element;
      stepper->next = 0;
      return 0;
   }else if( cur_index == (index-1) ){
      //Save the next node for later
      mem_t *next_node = stepper->next;
      stepper->next = element;
      stepper->next->next = next_node;
      return 0;
   }
   return -1;
}

//Returns the node given at index and removes
//it from the list. Returns null on error.
void *rm_node_ll(void **list, int index){

   if( *((mem_t**)list) == NULL || index < 0 )
      return 0;

   mem_t **head = (mem_t**)list;
   mem_t *stepper = (*head);

   //Handle the edge case of when we are removing the
   //first element. 
   if( index == 0 ){
      (*head) = (*head)->next;
      return (void*)stepper;
   }

   //Move through the list until we are at index-1
   int list_index = 0;
   while( list_index != (index-1) && stepper != NULL ){
      stepper = stepper->next;
      list_index++;
   }

   //Check to make sure we are at the correct location.
   //If not, the index we are searching for does not exist.
   if( list_index != (index - 1) ){
      return NULL;
   }

   //stepper is the current node. stepper->next
   //is the node to be removed. stepper->next->next
   //will end up in the place of the removed node.
   mem_t *new_next = stepper->next->next;
   mem_t *ret_node = stepper->next;

   stepper->next = new_next;
   return ret_node;
}


//Returns the node given at index but does
//not remove the node. Returns null on error
void *get_node_ll(void **list, int index){

   if( *((mem_t**)list) == NULL || index < 0 )
      return 0;

   mem_t **head = (mem_t**)list;
   mem_t *stepper = (*head);

   //Handle the edge case where we want the head node
   if( index == 0 ){
       return (void*)stepper;
   }

   //Move through the list until we are at index
   int list_index = 0;
   while( list_index != index && stepper != NULL ){
      stepper = stepper->next;
      list_index++;
   }

   //Check to make sure we are at the correct location.
   //If not, the index we are searching for does not exist.
   if( list_index != index ){
      return NULL;
   }

   //stepper is the current node. return it.
   return stepper;
}

//Replaces the node at index with new_node and
//returns the old node at that index. Returns null on error.
void *set_node_ll(void **list, void *new_node, int index){

   if( *((mem_t**)list) == NULL || new_node == NULL || index < 0 )
      return 0;

   mem_t **head = (mem_t**)list;
   mem_t *stepper = (*head);
   mem_t *new_head = (mem_t*)new_node;

   //Handle the edge case where we want to replace 
   //the head node
   if( index == 0 ){
      new_head->next = stepper->next;
      (*head) = new_head;
      return (void*)stepper;
   }

   //Move through the list until we are at index-1
   int list_index = 0;
   while( list_index != (index-1) && stepper != NULL ){
      stepper = stepper->next;
      list_index++;
   }

   //Check to make sure we are at the correct location.
   //If not, the index we are searching for does not exist.
   if( list_index != (index-1) ){
      return NULL;
   }

   mem_t *ret_node = stepper->next;
   new_head->next = stepper->next->next;
   stepper->next = new_head;

   return ret_node;
}

//Find the given element in the list, using a compare function
//passed in.  Return null if it is not found.
void *find_node_ll( void **list, void *element, int (*compare_function)(void *each_in_list, void *reference_node) ){

    mem_t **head = (mem_t**)list;
    mem_t *stepper = *head;

    while( stepper != NULL ){
       if( compare_function( stepper, element ) == 1 )
          return stepper;
       stepper = stepper->next;
    }
    return 0;
}

void apply_op_ll( void **list, void (*operator_function)(void*)){

   mem_t **head = (mem_t**)list;
   mem_t *next = *head;

   while( next != NULL ){
      operator_function( (void*)next );
      next = next->next;
   }
}

