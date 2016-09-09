/*
 * array.h
 *
 *  Created on: 8 sep. 2016
 *      Author: maximilianofelice
 */

#ifndef SRC_COLLECTIONS_ARRAY_H_
#define SRC_COLLECTIONS_ARRAY_H_

#include "iterable.h"

typedef struct array_metadata{
	int length;
} array_metadata;

void *_array_next(iterable* array);
int _array_has_next(iterable* array, array_metadata* metadata);
void _array_metadata_free(array_metadata* metadata);
void _array_metadata_init(array_metadata* metadata, size_t length);

iter_type(array, size_t)
#define ARRAY(length) array_type(length)

#endif /* SRC_COLLECTIONS_ARRAY_H_ */
