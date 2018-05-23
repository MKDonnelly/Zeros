#pragma once 

/*			Generic Linked List
   These functions comprise a data structure that is a greatly
   generalized linked list. An Generic Linked List (hereby abbreviated
   as GLL) has been designed to manage a linked list of structures
   of unknown size.  In contrast to some linked list implementations
   where data is stored in a void* in a node struct for the linked list,
   the data structures are the nodes. This means that this can be used
   to manage a linked list of any type of structure, subject to the only
   constraint that the first member of the linked list must be a pointer
   to a struct of the same type for use by these functions.

   The macros preceeding each function allows easier use of ALLs.
   Additionally, there are several other macros that allow easy processing.
*/

//Powerful looping macro. This simplifies many different operations
//on a linked list.
#define list_foreach( list, element ) \
        for( typeof(list) element = list_get_node( list, 0 ); \
             element;                                       \
             element = element->next )


//Create a lambda function.
#define LAMBDA( rettype, params, func )  \
        ({                               \
           rettype __lambda__ params {   \
               func;                     \
           }                             \
           __lambda__;                   \
        })

//This takes a pointer to a linked list and initilizes
//it with the given number of elements with the given size.
//Passing a size of zero just nulls out the list. If invalid
//arguments are passed, return -1
#define list_create( list, num_elements, element_size ) \
	__list_create( (void**)&list, num_elements, element_size )
void __list_create( void **list, int count, int element_size);


//Runs down a linked list and frees each node.
#define list_dealloc( list ) \
	__list_dealloc( (void**)&list )
void __list_dealloc(void **list);


//Adds an element to the list at the given index
#define list_add( list, element, index ) \
	__list_add( (void**)&list, element, index )
void __list_add( void **list, void *element, int index);


//Removes the given element from the list and returns it.
//Returns NULL on error
#define list_rm_index( list, index )\
	__list_rm_index((void**)&list, index)
void *__list_rm_index( void **list, int index );

//Removes the node given a pointer to it
#define list_rm_node( list, node )\
	__list_rm_node((void**)&list, node)
void __list_rm_node( void **list, void *node );


//Returns the given node at index. Returns NULL on error
//This macro simplifies the syntax by automatically casting
//the return pointer and transforming list into (void**)&list
#define list_get_node( list, index ) \
	((typeof(list))__list_get_node( (void**)&list, index ))
void *__list_get_node( void **list, int index );

//Replaces the node at the given index with the new node.
//Returns the old node.
#define list_replaceby_index( list, new_node, index ) \
	__list_replaceby_index( (void**)&list, new_node, index )
void *__list_replaceby_index( void **list, void *new_node, int index );

#define list_replaceby_node( list, new_node, old ) \
	__list_replaceby_node( (void**)&list, new_node, old )
void *__list_replaceby_node( void **list, void *new_node, void *old );

//Returns the size of the generic linked list
#define list_size( list )\
        __list_size( (void**)&list )
int __list_size(void **list);
