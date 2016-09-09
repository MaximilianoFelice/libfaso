/*
 * commons.c
 *
 *  Created on: 9 sep. 2016
 *      Author: maximilianofelice
 */
#include "commons.h"

int is_file(osada_file* file){
	return (file->state == REGULAR || file->state == DIRECTORY);
}

int file_matches(osada_file* file, char* token, uint16_t root){
	return (is_file(file) && strcmp((char*) file->fname, token) == 0 && (file->parent_directory == root));
}

uint16_t first_file_matching(osada_file* table, uint16_t offset, char* token, uint16_t root){
	if (offset > OSADA_FILE_TABLE_ENTRIES) {
		errno = -ENOENT;
		return NO_FILE;
	}
	else if (file_matches(table, token, root)) return offset;
	else return first_file_matching(table + 1, offset +1, token, root);
}

static int has_content(char* tok){
	return tok == NULL || tok[0] == '\0';
}

static uint16_t file_matching(char* path, uint16_t root){
	char* tok = strsep(&path, "/");

	if (has_content(path) && !has_content(tok)) return file_matching(path, root);

	else if (!has_content(path) && has_content(tok)) return first_file_matching(FILE_TABLE, 0, tok, root);

	else if (has_content(path) && has_content(tok)) file_matching(path, first_file_matching(FILE_TABLE, 0, tok, root));

	else return root;


}

uint16_t file_for_path(const char* path){
	char *freeable, *path_copy;
	freeable = path_copy = strdup(path);

	uint16_t parent = file_matching(path_copy, ROOT_DIR);

	free(freeable);
	return parent;
}

int first_free_file(osada_file* table, uint16_t offset){
	if (offset > OSADA_FILE_TABLE_ENTRIES) return -ENOENT;
	else if (!is_file(table)) return offset;
	else return first_free_file(table + 1, offset + 1);
}

osada_file* create_file(osada_block_pointer first_block,
				 uint32_t size, char* bname, uint16_t parent,
				 uint32_t lastmod, osada_file_state type){
	errno_clear;

	first_file_matching(FILE_TABLE, 0, bname, parent);
	switch(errno){
		case -ENOENT:
			errno_clear;
			break;
	}
	handle_return("Error: Probably found another file with that name");

	uint16_t block = first_free_file(FILE_TABLE, 0);
	handle_silent("Couldn't get free block");

	osada_file* dir = FILE_TABLE + block;

	dir->first_block = first_block;
	dir->file_size = size;
	memcpy(&(dir->fname), bname, strlen(bname));
	dir->parent_directory = parent;
	dir->lastmod = lastmod;
	dir->state = type;

	return dir;
}
