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

/*
typedef struct{
   list_head_t lh;
   int data;
}test_t;

void main(){
   list_t *test = list_create(test_t, lh);

   test_t *nt = malloc(sizeof(nt));
   nt->data = 111;

   test_t *second = malloc(sizeof(second));
   second->data = 222;

   list_pushfront(test, nt);
   list_pushfront(test, second);

   test_t *temp = list_getfront(test, test_t);
   printf("HERE: %d\n", temp->data);
   test_t *next = list_next_node( test, temp, test_t);
   printf("AGAIN: %d\n", next->data);
   test_list_foreach(test, iname, test_t){
      printf("Got %d\n", iname->data);
   }

   test_t *first = list_cconv(test, list_getfront(test), test_t);
   printf("Got %d\n", first->data);
   
   test_t *t = list_cconv(test, list_rmfront(test), test_t);
   printf("Again %d\n", t->data);

   test_t *ret = list_cconv(test, list_rmfront(test), test_t);
   printf("Got back %d\n", ret->data);

   test_t *val = list_cconv(test, list_rmfront(test), test_t);
   printf("Got %d\n", val->data);

   test_t *a = list_cconv(test, list_rmfront(test), test_t);
   if( a == NULL )
      printf("Works!\n");
}*/
