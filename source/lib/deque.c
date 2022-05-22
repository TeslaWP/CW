#include "deque.h"

deque * DQ_create(void){
	deque * L = malloc(sizeof(deque));
	L->data = null;
	L->dsize = 0;
	L->next = L;
	L->prev = L;
	return L;
}

int DQ_isempty(deque*L){
	return (L->next == L) ? 1 : null;
}

deque * DQ_create_elem(void* data, int size){
	deque * n = malloc(sizeof(deque));
	n->data = malloc(size);
	n->dsize = size;
	memcpy(n->data,data,size);
	n->next = null;
	n->prev = null;
	return n;
}

deque * DQ_last(deque * L){
	return DQ_isempty(L) ? null : L->prev;
}

deque * DQ_first(deque * L){
	return DQ_isempty(L) ? null : L->next;
}

void DQ_insert_after(deque * L, deque * p, deque * n){
	deque * x;
	n->next = p->next;
	n->prev = p;
	x = p->next;
	x->prev=n;
	p->next=n;
}

void DQ_insert_before(deque * L, deque * p, deque * n){
	deque * x;
	n->next = p;
	n->prev = p->prev;
	x = p->prev;
	x->next=n;
	p->prev=n;
}

void DQ_rpush(deque * L, void * data, int size){
	DQ_insert_before(L,L,DQ_create_elem(data, size));
}

void DQ_lpush(deque * L, void * data, int size){
	DQ_insert_after(L,L,DQ_create_elem(data, size));
}

void DQ_destroy(deque * L) {
	while (!DQ_isempty(L)) {
		DQ_rpop(L, (void*)0);
	}
	free(L);
}

void DQ_delete(deque * L, deque * n, void * dest){
	if (n == L) return;
	deque * left = n->prev;
	deque * right = n->next;
	left->next = right;
	right->prev = left;
	if (dest!=0) memcpy(dest,n->data,n->dsize);
	free(n->data);
	free(n);
	return;
}

void DQ_lpop(deque * L, void * dest){
	if (DQ_isempty(L)) return;
	DQ_delete(L,DQ_first(L),dest);
}

void DQ_rpop(deque * L, void * dest){
	if (DQ_isempty(L)) return;
	DQ_delete(L,DQ_last(L),dest);
}