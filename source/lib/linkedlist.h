#ifndef HDR_LINKEDLIST_H
#define HDR_LINKEDLIST_H

#include <stdlib.h>

#define null 0

typedef struct SL_list_node {
	void* data;
	struct SL_list_node *next;
} SL_list;

SL_list * SL_last(SL_list*L);
SL_list * SL_first(SL_list*L);
void SL_insert_after(SL_list*L,SL_list*p,SL_list*n);
void SL_add_first(SL_list*L,SL_list*n);
void SL_insert_before(SL_list*L, SL_list*p, SL_list*n);
SL_list * SL_remove(SL_list*L, SL_list*n);
SL_list * SL_create(void* data);
SL_list * SL_destroy(SL_list*L);
SL_list * SL_print(SL_list*L);
int SL_count(SL_list*L);
#endif