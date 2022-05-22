#include "linkedlist.h"

SL_list * SL_last(SL_list*L){
	while (L->next) L = L-> next;
	return L;	
}

SL_list * SL_first(SL_list*L){
	return L;
}

void SL_insert_after(SL_list*L,SL_list*p,SL_list*n){
	n->next = p->next;
	p->next = n;
}

void SL_add_first(SL_list*L,SL_list*n) {
	SL_insert_after(L,L,n);
}

void SL_insert_before(SL_list*L, SL_list*p, SL_list*n){
	if (n == L || n->next  != null) return;
	SL_insert_after(L,p,n);
	void* tmp = p->data; p->data = n->data;
	n->data = tmp;
}

SL_list * SL_remove(SL_list*L, SL_list*n){
	if (n == L || n->next  == null) return 0;
	SL_list * r = n->next;
	void* data = r->data; r->data = n->data; n->data = data;
	n->next = r->next; r->next = null;
	return r;
}

SL_list * SL_create(void* data){
	SL_list *n = malloc(sizeof(SL_list));
	n->next = null;
	n->data = data;
	return n;
}

SL_list * SL_destroy(SL_list*L){
	while (L != null){
		SL_list * n = L->next;
		free(n);
		L = n;
	}
	return null;
}

int SL_count(SL_list*L){
	int counter = 0;
	while (L->next) {
		L=L->next;
		counter++;
	}
	return counter;
}