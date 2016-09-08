/*
 * iterable.c
 *
 *  Created on: 8 sep. 2016
 *      Author: maximilianofelice
 */

#include "../collections/iterable.h"

#include <stdio.h>
#include <string.h>

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
		_##type##_init_metadata(type->metadata, args); \
		\
		return type; \
	}

#define iter_type(type, definition) \
	__iter_has_next(type) \
	__iter_next(type) \
	__iter_metadata_free(type) \
	__iter_metadata(type, definition)

/** ARRAY IMPLEMENTATION **/
typedef struct array_metadata{
	int length;
} array_metadata;


void *_array_next(iterable* array){
	return array->head + array->struct_size;
}

int _array_has_next(iterable* array, array_metadata* metadata){
	return (array->offset < metadata->length);
}

void _array_metadata_free(array_metadata* metadata){}

void _array_init_metadata(array_metadata* metadata, size_t length){
	metadata->length = length;
}

iter_type(array, size_t)
#define ARRAY(length) array_type(length)

/** END ARRAY IMPLEMENTATION **/


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

int main(){
	char* str = strdup("hello");

	iterable* it = create_iterator(str, ARRAY(strlen(str)));

	while (has_next(it)){
		printf("%c\n", ((char*)next(it))[0]);
	}

	free_iterator(it);
	return 0;
}
