#include <lib/genericll.h>
#include <lib/types.h>
#include <kernel/mm/heap.h>

list_t *__list_create(int ptr_offset){
   list_t *new_list = k_malloc(sizeof(list_t), 0);
   new_list->head.next = new_list->head.prev = NULL;
   new_list->len = 0;
   new_list->ptr_offset = ptr_offset;
   return new_list;
}

//Adding always appends to the front of the list
void list_pushfront(list_t *list, void *item){
   list_head_t *new_item = (list_head_t*)((char*)item + list->ptr_offset);
   list_head_t *first_item = list->head.next;

   if( first_item != NULL )
      first_item->prev = new_item;
   list->head.next = new_item;

   new_item->next = first_item;
   new_item->prev = &list->head;

   list->len++;
}

void __list_rm(list_t *list, list_head_t *node){
   list_head_t *prev = node->prev;
   list_head_t *next = node->next;
   
   if( prev != NULL )
      prev->next = next;

   if( next != NULL )
   next->prev = prev;

   list->len--;
}

//This can be converted the the container's type using 
//list_cconv (list container convert)
list_head_t *__list_rmfront(list_t *list){
   list_head_t *ret = list->head.next;
   list_rm(list, ret);
   return ret;
}

list_head_t *__list_getfront(list_t *list){
   return list->head.next;
}
