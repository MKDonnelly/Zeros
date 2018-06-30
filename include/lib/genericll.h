#pragma once 

#define list_next( list, node ) \
        (node)->next

//Given a list_head_t*, get the container it is in
#define list_node_cont( list, node, type ) \
        (type*)( (char*)node - (list)->ptr_offset )

//Given a node, get the list_head_t* within it
#define list_node_ptr( list, node ) \
        (list_head_t*)( (char*)node + (list)->ptr_offset )

//Take the node, grab it containing pointer, jump through that 
//pointer, then convert back to the node type.
#define list_next_node( list, node, type ) \
        list_node_cont(list, list_next(list, \
                             list_node_ptr( list, node )), type)

//Get the list length
#define list_len( list ) \
        (list)->len

#define list_foreach( list, iter_name, type ) \
        for( type *iter_name = list_getfront(list, type);	\
             iter_name != NULL;					\
             iter_name = list_next_node(list, iter_name, type))

//Statically initilize a list_t
#define LIST_SINIT(offset) \
        {.head.next = NULL, .head.prev = NULL, .len = 0, \
         .ptr_offset = offset}

//Structs to be put in the list put this at
//the head of the structure.
typedef struct list_head{
   struct list_head *next;
   struct list_head *prev;
}list_head_t;

typedef struct list{
   list_head_t head;
   int len;
   int ptr_offset;
}list_t;

#define list_create( struct_name, struct_member ) \
        __list_create( __builtin_offsetof(struct_name, struct_member) )
list_t *__list_create(int ptr_offset);

void list_pushfront(list_t *list, void *item);

//__list_rm works with list_head_t*, instead of void*.
//This converts it
#define list_rm( lst, node ) \
        __list_rm( lst, list_node_ptr(lst, node) )
void __list_rm(list_t *list, list_head_t *node);

//__list_rmfront returns a list_head_t*. Convert it
#define list_rmfront(lst, type) \
        (list_node_cont( lst, __list_rmfront(lst), type))
list_head_t *__list_rmfront(list_t *list);

//__list_getfront returns a list_head_t*. Convert it
#define list_getfront(lst, type) \
        (list_node_cont( lst, __list_getfront(lst), type))
list_head_t *__list_getfront(list_t *list);
