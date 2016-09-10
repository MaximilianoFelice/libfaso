/*
 * ioopts.c
 *
 *  Created on: 8 sep. 2016
 *      Author: maximilianofelice
 */
#include "../osada.h"
#include "commons.h"

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;
	if(strcmp(path, hello_path) != 0)
		return -ENOENT;

	len = strlen(hello_str);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, hello_str + offset, size);
	} else
		size = 0;

	return size;
}

int osada_mknod(const char *path, mode_t st_mode, dev_t type){
	if (S_ISDIR(st_mode)) return osada_create_file(path, DIRECTORY);
	else if (S_ISREG(st_mode)) return osada_create_file(path, REGULAR);
	else return errno = -EINVAL;
}


int osada_truncate(const char *path, off_t new_size){
	errno_clear;

	uint16_t block = file_for_path(path);
	handle_return("Cannot find file");

	osada_file* file = FILE_TABLE + block;
	set_in_file(file, file->first_block, new_size, file->fname, file->parent_directory, time(NULL), file->state);
	handle_return("Couldn't update file");

	return 0;
}

typedef uint16_t traverse_mode;
#define TR_READ 0x1
#define TR_WRITE 0x2

osada_block_pointer alloc_block(){
	int block = 0;
	while(bitarray_test_bit(bitmap, block)){
		if (block > get_zone(disk, DISK_DATA)->size){
			errno = -1;
			return LAST_BLOCK;
		}
		block++;
	}
	return block;
}

int handle_block_creation(osada_block_pointer *from, traverse_mode mode){
	if (*from != LAST_BLOCK) return 0;
	if(TEST_FLAG(mode, TR_WRITE)) {
		osada_block_pointer p = alloc_block();
		handle_return("Disk is full");
		*from = p;
		return p;
	}
	else return errno = -1;
}

osada_block* link_block(osada_file* file, off_t offset, traverse_mode mode){
	handle_block_creation((osada_block_pointer*) &(file->first_block), mode);
	handle_return_null("Couldn't create block");
	osada_block_pointer from = file->first_block;

	int steps = offset / OSADA_BLOCK_SIZE;

	do {
		handle_block_creation(ALLOC_TABLE + from, mode);
		handle_return_null("Couldn't create block");

		if (steps > 0) from = ALLOC_TABLE[from];
		steps--;
	} while (steps > 0);

	return DATA + from;
}

off_t block_fill(char* to, char* from, off_t offset){
	off_t to_write = OSADA_BLOCK_SIZE - offset;
	memcpy(to, from, to_write);
	return to_write;
}

int check_file_size(osada_file *file, size_t size, traverse_mode mode){
	if (file->file_size > size) return 0;
	if (TEST_FLAG(mode, TR_WRITE)) return file->file_size = size;
	else return errno = -1;
}

int osada_traverse(const char *path, char* buffer, size_t size, off_t offset,
				   traverse_mode mode){
	errno_clear;

	uint16_t block = file_for_path(path);
	handle_return("Cannot find file");
	osada_file* file = FILE_TABLE + block;

	check_file_size(file, offset + size, mode);
	handle_return("File offset exceeded");

	off_t advance = 0;
	off_t position, left_to_operate = size;

	while(left_to_operate > 0){
		advance = size - left_to_operate;
		position = offset + advance;

		osada_block *block = link_block(file, position, mode);
		handle_return("Cannot read more than file length");

		if (TEST_FLAG(mode, TR_WRITE))
			left_to_operate -= block_fill((char*) block, buffer + advance, position % OSADA_BLOCK_SIZE);
		else
			left_to_operate -= block_fill(buffer + advance, (char*) block, position % OSADA_BLOCK_SIZE);

		handle_return("Cannot fill block");
	}

	return advance;
}
int osada_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi){
	return osada_traverse(path, buffer, size, offset, TR_READ);
}

int osada_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *fi){
	return osada_traverse(path, (char*) buffer, size, offset, TR_WRITE);
}
