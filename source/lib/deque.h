#ifndef HDR_DEQUE_H
#define HDR_DEQUE_H

#include <stdlib.h>
#include <string.h>

#define null 0

typedef struct deque_node {
	void* data;
	int dsize;
	struct deque_node * next;
	struct deque_node * prev;
} deque;

deque * DQ_create(void);
int DQ_isempty(deque*L);
deque * DQ_create_elem(void* data, int size);
deque * DQ_last(deque * L);
deque * DQ_first(deque * L);
void DQ_insert_after(deque * L, deque * p, deque * n);
void DQ_insert_before(deque * L, deque * p, deque * n);
void DQ_rpush(deque * L, void * data, int size);
void DQ_lpush(deque * L, void * data, int size);
void DQ_delete(deque * L, deque * n, void * dest);
void DQ_destroy(deque * L);
void DQ_lpop(deque * L, void * dest);
void DQ_rpop(deque * L, void * dest);
#endif