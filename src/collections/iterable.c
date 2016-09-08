/*
 * iterable.c
 *
 *  Created on: 8 sep. 2016
 *      Author: maximilianofelice
 */

#include "iterable.h"

iterable* __create_iterator(void* head, size_t struct_size, __iter_type *type){
	iterable* iterator = malloc(sizeof(iterable));

	iterator->head = head;
	iterator->offset = 0;
	iterator->struct_size = struct_size;
	iterator->type = type;

	return iterator;
}

void free_iterator(iterable* iterator){
	iterator->type->free_type(iterator->type);
	free(iterator);
}

int has_next(iterable* iterator){
	return iterator->type->has_next(iterator);
}

void* next(iterable* iterator){
	if (!has_next(iterator)) return NULL;
	void* elem = iterator->head;
	iterator->head = iterator->type->next(iterator);
	iterator->offset += 1;
	return elem;
}
