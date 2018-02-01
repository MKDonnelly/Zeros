#pragma once

#include <kmalloc.h>
#include <types.h>


typedef struct llnode{
   uint32_t *item;
   struct llnode *next;
} llnode_t;

void create_ll( llnode_t **head, int size );

void add_element_ll( llnode_t **head, int index, uint32_t *val);

//Note: This function passes back the pointer to the item in case
//      it needs to be freed. Ignoring the return value could be used
//      to get and delete the item at the given index.
uint32_t *rm_element_ll( llnode_t **head, int index);

void set_item_ll( llnode_t **head, int index, uint32_t *val);

uint32_t *get_item_ll( llnode_t **head, int index);
