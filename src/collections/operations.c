/*
 * operations.c
 *
 *  Created on: 8 sep. 2016
 *      Author: maximilianofelice
 */

#include "operations.h"

/*
typedef struct from_metadata{
} from_metadata;

void *_fold_next(iterable* array){
	return array->head + array->struct_size;
}

int _fold_next(iterable* array, array_metadata* metadata){
	return (array->offset < metadata->length);
}

void _fold_metadata_free(array_metadata* metadata){}

void _fold_metadata_init(array_metadata* metadata){
}

iter_type(array, size_t)
#define FOLD() fold_type()

*/
void reset(iterable* iterator){
	iterator->head = (void*) iterator->first;
	iterator->offset = 0;
}
/*
iterable* fold(iterable* source, void* seed, void*(*op)(void*, void*)){
	return create_iterator(source->head, source->struct_size, FOLD())
}
*/
