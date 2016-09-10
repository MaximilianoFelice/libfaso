/*
 * diropts.c
 *
 *  Created on: 7 sep. 2016
 *      Author: maximilianofelice
 */
#include "../osada.h"
#include "commons.h"

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
	return osada_create_file(path, DIRECTORY);
}

int osada_open(const char *path, struct fuse_file_info *fi){
	errno_clear;

	file_for_path(path);
	handle_return("Cannot open directory");

	return 0;
}

int osada_rmdir (const char* path){
	errno_clear;

	if (strcmp(path, ".") == 0 || strcmp(path, "..") == 0){
		errno = -EBUSY;
	}
	handle_return("Couldn't remove dir");

	uint16_t block = file_for_path(path);
	handle_return("Cannot find path");

	if (!directory_is_empty(block)) errno = -ENOTEMPTY;
	handle_return("Directory is not empty");

	osada_file *file = FILE_TABLE + block;
	file->state = DELETED;

	return 0;
}

int osada_rename (const char* oldpath, const char* newpath){
	errno_clear;

	uint16_t file_block = file_for_path(oldpath);
	handle_return("Cannot find file");

	file_for_path(newpath);
	if (errno == 0) errno = -EEXIST;
	else errno = 0;
	handle_return("File already exists");

	char* dname = dirname(strdup(newpath));
	char* bname = basename(strdup(newpath));

	uint16_t parent = file_for_path(dname);
	handle_return("Cannot find destiny");

	osada_file* file = FILE_TABLE + file_block;
	set_in_file(file, file->first_block, file->file_size, (unsigned char*) bname, parent, time(NULL), file->state);
	handle_return("Couldn't fully update file");

	return 0;

}

int osada_utimens(const char *path, const struct timespec tv[2]){
	errno_clear;

	uint16_t block = file_for_path(path);
	handle_return("Cannot find file");

	osada_file* file = FILE_TABLE + block;
	set_in_file(file, file->first_block, file->file_size, file->fname, file->parent_directory, tv[1].tv_sec, file->state);
	handle_return("Cannot update file");

	return 0;
}
