#include "iterable.h"

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
