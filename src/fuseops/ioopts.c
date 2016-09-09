/*
 * ioopts.c
 *
 *  Created on: 8 sep. 2016
 *      Author: maximilianofelice
 */
#include "../osada.h"

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

int osada_mknod (const char *path, mode_t st_mode, dev_t type){
	if (S_ISDIR(st_mode)) return osada_create_file(path, DIRECTORY);
	else if (S_ISREG(st_mode)) return osada_create_file(path, REGULAR);
	else return errno = -EINVAL;
}
