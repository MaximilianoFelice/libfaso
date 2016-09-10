/*
 * commons.h
 *
 *  Created on: 9 sep. 2016
 *      Author: maximilianofelice
 */

#include "../osada.h"

#ifndef SRC_FUSEOPS_COMMONS_H_
#define SRC_FUSEOPS_COMMONS_H_

int is_file(osada_file* file);
int file_matches(osada_file* file, char* token, uint16_t root);

uint16_t first_file_matching(osada_file* table, uint16_t offset, char* token, uint16_t root);
uint16_t file_for_path(const char* path);

int first_free_file(osada_file* table, uint16_t offset);

int set_in_file(osada_file* dir, osada_block_pointer first_block,
				 uint32_t size, unsigned char* bname, uint16_t parent,
				 uint32_t lastmod, osada_file_state type);

osada_file* create_file(osada_block_pointer first_block,
				 uint32_t size, unsigned char* bname, uint16_t parent,
				 uint32_t lastmod, osada_file_state type);

int directory_is_empty(uint16_t parent);

int osada_create_file(const char* path, osada_file_state type);
int osada_delete_file(const char* path);

#endif /* SRC_FUSEOPS_COMMONS_H_ */
