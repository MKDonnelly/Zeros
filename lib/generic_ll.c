#include <lib/generic_ll.h>

//Internally used and NEVER used by outside code.
//This is meant to cast an unknown list element into
//something that we can work with. 
typedef struct mem{
   struct mem *next;
}mem_t;


//Given a pointer to an element of the type we are using,
// create a linked list with 'count' items, each of size
// 'element_size'. Passing 0 for size nulls the list.
//ex.
//   my_crazy_structure *my_structure;
//   create_ll( my_structure, 20, sizeof(my_crazy_structure));
//              ^^^^^^^^^^^^ Valid because of macro in header
int __create_ll( void **list, int count, int element_size){

   //Validate arguments. If any are invalid, return null
   if( list == 0 || count < 0 || element_size < 0 ){
      return -1;
   }

   //A ALL of size zero is a null list
   if( count == 0 || element_size == 0 ){
      *((mem_t**)list) = NULL;
      return 0;
   }

   //Initilize the first element.
   mem_t **head = (mem_t**)list;
   (*head) = (mem_t*)k_malloc(element_size, 0);

   mem_t *next_node = (*head);

   //While there are still more elements to create
   while( --count > 0 ){

      //Allocate the *next item in the list and jump through it.
      next_node->next = (mem_t*)k_malloc( element_size, 0 );
      next_node = next_node->next;
   }

   //Make sure the next pointer of the last node is null
   next_node->next = NULL;
}


//Deallocates each item in the linked list.
//ex.
//   my_crazy_structure *list;
//   ...
//   dealloc_ll( list );
void __dealloc_ll(void **list){

   
   mem_t **list_addr = (mem_t**)list;

   mem_t *head = (mem_t*)(*list_addr);
   mem_t *next = (mem_t*)(*list_addr);
  
   //Run down the linked list by freeing the head node
   //and then stepping to next. 
   while( head != NULL ){
      next = head->next;
      k_free( head );
      head = next;
   }
}


//Adds element at index in list. Returns -1 on error
//ex.
//    my_crazy_structure *list;
//    add_node_ll( list, new_element, 0 );
int __add_node_ll( void **list, void *element, int index){

   //Return if the element is null or the index is bad
   //Also return if we are asked to place an element
   //at index != 0 with the head null
   if( (element == 0 || index < 0) || 
       ( *((mem_t**)list) == NULL && index != 0  ) )
      return -1;

   mem_t **head = (mem_t**)list;

   //Adding an element to the head is a special case
   //We need to modify the actual pointer to the list passed in.
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
void *__rm_node_ll(void **list, int index){

   if( *((mem_t**)list) == NULL || index < 0 )
      return NULL;

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
void *__get_node_ll(void **list, int index){

   if( *((mem_t**)list) == NULL || index < 0 )
      return NULL;

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
void *__set_node_ll(void **list, void *new_node, int index){

   if( *((mem_t**)list) == NULL || new_node == NULL || index < 0 )
      return NULL;

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

int __get_size_ll( void **list ){
   mem_t **head = (mem_t**)list;
   mem_t *element = *head;
   int total = 0;

   while( element != NULL ){
      element = element->next;
      total++;
   }
   return total;
}
