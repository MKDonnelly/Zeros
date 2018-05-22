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
#define foreach_in_gll( list, element ) \
        for( typeof(list) element = get_node_ll( list, 0 ); \
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
#define create_ll( list, num_elements, element_size ) \
	__create_ll( (void**)&list, num_elements, element_size )
void __create_ll( void **list, int count, int element_size);


//Runs down a linked list and frees each node.
#define dealloc_ll( list ) \
	__dealloc_ll( (void**)&list )
void __dealloc_ll(void **list);


//Adds an element to the list at the given index
#define add_node_ll( list, element, index ) \
	__add_node_ll( (void**)&list, element, index )
void __add_node_ll( void **list, void *element, int index);


//Removes the given element from the list and returns it.
//Returns NULL on error
#define rmi_node_ll( list, index )\
	__rmi_node_ll((void**)&list, index)
void *__rmi_node_ll( void **list, int index );

//Removes the node given a pointer to it
#define rmp_node_ll( list, ptr )\
	__rmp_node_ll((void**)&list, ptr)
void __rmp_node_ll( void **list, void *ptr );


//Returns the given node at index. Returns NULL on error
//This macro simplifies the syntax by automatically casting
//the return pointer and transforming list into (void**)&list
#define get_node_ll( list, index ) \
	((typeof(list))__get_node_ll( (void**)&list, index ))
void *__get_node_ll( void **list, int index );

//Replaces the node at the given index with the new node.
//Returns the old node.
#define replacei_node_ll( list, new_node, index ) \
	__replacei_node_ll( (void**)&list, new_node, index )
void *__replacei_node_ll( void **list, void *new_node, int index );

#define replacep_node_ll( list, new_node, old ) \
	__replacep_node_ll( (void**)&list, new_node, old )
void *__replacep_node_ll( void **list, void *new_node, void *old );

//Returns the size of the generic linked list
#define get_size_ll( list )\
        __get_size_ll( (void**)&list )
int __get_size_ll(void **list);
