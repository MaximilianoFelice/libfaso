/*
 * iterable.h
 *
 *  Created on: 8 sep. 2016
 *      Author: maximilianofelice
 */

#ifndef SRC_COLLECTIONS_ITERABLE_H_
#define SRC_COLLECTIONS_ITERABLE_H_

#include <stdlib.h>
#include <sys/types.h>

typedef void*(*iterable_next)(void);
typedef int iterable_has_next;

typedef void*(*iter_next)(void*);
typedef int(*iter_has_next)(void*);

typedef struct __iter_type{
	iter_next next;
	iter_has_next has_next;
	void(*free_type)(struct __iter_type*);
	void* metadata;
} __iter_type;

typedef struct iterable {
	void* head;
	size_t struct_size;
	int offset;
	__iter_type* type;
} iterable;

#define create_iterator(head, type) \
	__create_iterator((void*) head, sizeof(*head), type)

int has_next(iterable* iterator);
void* next(iterable* iterator);

#endif /* SRC_COLLECTIONS_ITERABLE_H_ */
