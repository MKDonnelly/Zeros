#include <linked_list.h>

void create_ll( llnode_t **head, int size ){
   if( size <= 0 ){
      *head = NULL;
      return;
   }

   //Create the first element
   *head = (llnode_t*)kmalloc(sizeof(llnode_t), 0, 0);
   (*head)->next = NULL;
   size--;

   llnode_t *cur_node = *head;

   //Create each element
   while( size > 0 ){
      cur_node->next = (llnode_t*)kmalloc(sizeof(llnode_t), 0, 0);
      cur_node = cur_node->next;
      cur_node->next = NULL;
      cur_node->item = 0;
      size--;
   }
}

void add_element_ll( llnode_t **head, int index, uint32_t *item ){

   //Catch some specific cases,
   // Null list w/ index of 0
   // Null list w/ out of range index
   // Index of 0 w/ any list size
   if( *head == NULL && index == 0 ){
      *head = (llnode_t*)kmalloc(sizeof(llnode_t), 0, 0);
      (*head)->next = NULL;
      (*head)->item = item;
      return;
   }else if( *head == NULL ){
      return;
   }else if( index == 0 ){
      llnode_t *new_head = (llnode_t*)kmalloc(sizeof(llnode_t), 0, 0);
      new_head->item = item;
      new_head->next = *head;
      *head = new_head;
      return;
   }

   llnode_t *cur_node = *head;
   int running_index = 0;

   while( cur_node->next != NULL && running_index != index-1 ){
      cur_node = cur_node->next;
      running_index++;
   }
   
   //Double-check to make sure we are at the right spot
   if( running_index == index-1 ){
      //Save the next node
      llnode_t *next_node = cur_node->next;

      cur_node->next = (llnode_t*)kmalloc(sizeof(llnode_t), 0, 0);
      cur_node->next->item = item;
      cur_node->next->next = next_node;
   }
}

uint32_t *rm_element_ll( llnode_t **head, int index){

   //Handle some special cases
   // List is null
   // One-element list
   // Removing first element
   if( *head == NULL){
      return NULL;
   }else if( (*head)->next == NULL ){
      uint32_t *item = (*head)->item;
      kfree( *head );
      *head = NULL;
      return item;
   }else if( index == 0 ){
      uint32_t *item = (*head)->item;
      llnode_t *saved_node = *head;
      (*head) = (*head)->next;
      kfree( saved_node );
      return item;
   }


   uint32_t *ret_item = NULL;   
   llnode_t *cur_item = *head;
   int cur_index = 0;

   //Go to the element just before the one to delete
   while( cur_item->next != NULL && cur_index != index-1){
      cur_item = cur_item->next;
      cur_index++;
   }

   //Double check to make sure we landed at the right spot
   if( cur_index == index-1 ){
      //If the element to remove is the last one
      if( cur_item->next->next == NULL ){
         ret_item = cur_item->next->item;
         kfree( cur_item->next );
      }else{
         ret_item = cur_item->next->item;
         llnode_t *next_item = cur_item->next->next;
         kfree( cur_item->next );
         cur_item->next = next_item;
      }
   }
   return ret_item;
}

void set_item_ll( llnode_t **head, int index, uint32_t *val){

   int cur_index = 0;
   llnode_t *cur_node = *head;

   while( cur_node->next != NULL && cur_index != index ){
      cur_node = cur_node->next;
      cur_index++;
   }
      
   //Double check to make sure we landed in the right spot
   if( cur_index == index ){
      cur_node->item = val;
   }
}


uint32_t *get_item_ll( llnode_t **head, int index ){

   //Handle the special case if we are passed
   //and empty list.
   if( *head == NULL ){
      return NULL;
   }
   
   int cur_index = 0;
   llnode_t *cur_node = *head;

   while( cur_node->next != NULL && cur_index != index ){
      cur_node = cur_node->next;
      cur_index++;
   }
      
   //Double check to make sure we landed in the right spot
   if( cur_index == index ){
      return cur_node->item;
   }else{
      return 0;
   }
}
