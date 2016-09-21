/*
 * ioopts.c
 *
 *  Created on: 8 sep. 2016
 *      Author: maximilianofelice
 */
#include "commons.h"

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
	int padding = bitmap_padding();
	int block = padding;
	while(bitarray_test_bit(bitmap, block)){
		block++;
		if (block >= disk->block_count){
			errno = -ENOSPC;
			return LAST_BLOCK;
		}
	}
	bitarray_set_bit(bitmap, block);
	return block - padding;
}

osada_block_pointer link_block(osada_block_pointer *from){
	if (*from != LAST_BLOCK) return errno = -1;
	osada_block_pointer p = alloc_block();
	handle_return_silent("Disk is full");
	*from = p;
	ALLOC_TABLE[p] = LAST_BLOCK;
	return p;
}


osada_block_pointer *find_last_block(osada_file *file){
	osada_block_pointer *p = NULL;
	int _keep_last(osada_block_pointer *block, int count){
		p = block;
		return 1;
	}
	iterate_blocks(file, _keep_last);
	return p;
}

off_t block_fill(char* to, char* from, size_t size, off_t offset){
	off_t to_write = size - offset;
	memcpy(to, from, to_write);
	return to_write;
}

int check_file_size(osada_file *file, size_t size, traverse_mode mode){
	if (file->file_size > size) return size;
	if (TEST_FLAG(mode, TR_WRITE)) return file->file_size = size;
	else {
		errno = -1;
		return file->file_size;
	}
}

int osada_traverse(const char *path, char* buffer, size_t size, off_t offset,
				   struct fuse_file_info *fi, traverse_mode mode){
	errno_clear;

	uint16_t block = file_for_path(path);
	handle_return("Cannot find file");
	osada_file* file = FILE_TABLE + block;

	size = check_file_size(file, offset + size, mode) - offset;
	if (fi->direct_io)
	{handle_return("File offset exceeded");
	} else errno=0;


	off_t left_to_operate = size;
	off_t operated = 0;
	off_t position = offset;

	int _is_required_node(){
		if (	(int) position > ((int)offset - (int)disk->block_size) &&
				(int) position <= ((int)size + (int)offset)) return 1;
		else return 0;
	}

	int _continue_linking(osada_block_pointer *p){
		if (left_to_operate <= 0 || position > (size + offset)) return 0;
		else if (*p != LAST_BLOCK) return 1;
		else if (TEST_FLAG(mode, TR_WRITE)) {
			link_block(p);
			return errno == 0;
		} else {
			errno = -1;
			return 0;
		}
	}

	size_t _size_to_operate(){
		if (left_to_operate >= disk->block_size) return disk->block_size;
		else return left_to_operate;
	}

	int _operate(osada_block_pointer *p, int count){
		position = count * disk->block_size;

		int link = _continue_linking(p);
		if (errno != 0) return 0;

		if (_is_required_node() && link){
			if (TEST_FLAG(mode, TR_WRITE))
				left_to_operate -= block_fill((char*) (DATA + *p), buffer + operated, _size_to_operate(), position % OSADA_BLOCK_SIZE);
			else
				left_to_operate -= block_fill(buffer + operated, (char*) (DATA + *p), _size_to_operate(), position % OSADA_BLOCK_SIZE);

			operated = size - left_to_operate;
		}

		return link && (left_to_operate > 0);
	}


	iterate_blocks(file, _operate);
	handle_return_silent("There was an error operating over blocks");

	return operated;

}
int osada_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi){
	return osada_traverse(path, buffer, size, offset, fi, TR_READ);
}

int osada_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *fi){
	return osada_traverse(path, (char*) buffer, size, offset, fi, TR_WRITE);
}
