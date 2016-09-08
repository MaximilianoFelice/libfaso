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

#define __iter_has_next(_type) \
	int __##_type##_has_next(void* iterator){ \
		iterable* it = (iterable*) iterator; \
		__iter_type *type = it->type; \
		_type##_metadata *metadata = (_type##_metadata*) type->metadata; \
		return _##_type##_has_next(it, metadata); \
	}

#define __iter_next(type) \
	void *__##type##_next(void* iterator){ \
		iterable* it = (iterable*) iterator; \
		if (!has_next(it)) return NULL; \
		return _##type##_next(it); \
	}

#define __iter_metadata_free(type) \
	void __##type##_metadata_free(__iter_type* type){ \
		type##_metadata* metadata = (type##_metadata*) type->metadata; \
		_##type##_metadata_free(metadata); \
		free(type->metadata); \
		free(type); \
	}

#define __iter_metadata(type, args_type) \
	__iter_type *type##_type(args_type args){ \
		__iter_type *type = malloc(sizeof(__iter_type)); \
		type##_metadata *metadata = malloc(sizeof(type##_metadata)); \
		\
		type->free_type = __##type##_metadata_free; \
		type->next = __##type##_next; \
		type->has_next = __##type##_has_next; \
		type->metadata = metadata; \
		_##type##_metadata_init(type->metadata, args); \
		\
		return type; \
	}

#define iter_type(type, definition) \
	__iter_has_next(type) \
	__iter_next(type) \
	__iter_metadata_free(type) \
	__iter_metadata(type, definition)


#endif /* SRC_COLLECTIONS_ITERABLE_H_ */
