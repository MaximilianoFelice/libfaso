/*
 * diropts.c
 *
 *  Created on: 7 sep. 2016
 *      Author: maximilianofelice
 */
#include "../osada.h"
#include "commons.h"

static const char *hello_path = "/hello";

static int fill_root_stbuf(struct stat *stbuf){
	stbuf->st_mode = S_IFDIR | 0777;
	stbuf->st_nlink = 0;
	stbuf->st_size = 0x0;
	stbuf->st_mtime = time(NULL);
	stbuf->st_atime = time(NULL);
	return 0;
}

int osada_getattr(const char *path, struct stat *stbuf){
	errno_clear;

	/* Root has no block defined on FS */
	if (strcmp(path, "/") == 0){
		fill_root_stbuf(stbuf);
		handle_return("Couldn't fill root dir attrs");
		return 0;
	}

	uint16_t block = file_for_path(path);
	handle_return("Cannot find block for dirname");

	osada_file* file = FILE_TABLE + block;

	memset(stbuf, 0, sizeof(struct stat));

	switch(file->state){
		case DIRECTORY:
			stbuf->st_mode = S_IFDIR | 0777;
			break;
		case REGULAR:
			stbuf->st_mode = S_IFREG | 0777;
			break;
		default:
			errno = -ENOENT;
	}
	handle_return("The file was no file");

	stbuf->st_nlink = 0;
	stbuf->st_size = file->file_size;
	stbuf->st_mtime = file->lastmod;
	stbuf->st_atime = time(NULL);

	return 0;
}

int osada_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
	errno_clear;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

	uint16_t parent = file_for_path(path);
	handle_return("Cannot find block for dirname");

	for(int i = 0; i < OSADA_FILE_TABLE_ENTRIES; i++){
		if (is_file(FILE_TABLE + i) && FILE_TABLE[i].parent_directory == parent){
			filler(buf, (char*) FILE_TABLE[i].fname, NULL, 0);
		}
	}

	return 0;
}

int osada_mkdir(const char* path, mode_t mode){
	errno_clear;

	char* bname = basename(strdup(path));
	char* dname = dirname(strdup(path));

	uint16_t parent = file_for_path(dname);
	handle_return("Cannot find block for dirname");

	create_file(ROOT, 0x0, bname, parent, time(NULL), DIRECTORY);
	handle_return("Cannot create file");

	return 0;

}

int osada_open(const char *path, struct fuse_file_info *fi)
{
	file_for_path(path);
	handle_return("Cannot open directory");

	return 0;
}
