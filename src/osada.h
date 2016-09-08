/*
 * osada.h
 *
 *  Created on: 24 ago. 2016
 *      Author: maximilianofelice
 */

#ifndef SRC_OSADA_H_
#define SRC_OSADA_H_

#define FUSE_USE_VERSION 29

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "utils/mapping.h"
#include "utils/errors.h"
#include "../osada-tools/osada.h"
#include "utils/fuseopts.h"

/*
 * Este es el path de nuestro, relativo al punto de montaje, archivo dentro del FS
 */
#define DEFAULT_FILE_PATH "/" DEFAULT_FILE_NAME

#define OSADA_FILE_TABLE_BLOCKS 1024

extern Disk* disk;

/* FUSE Operations */
int hello_getattr(const char *path, struct stat *stbuf);
int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi);
int hello_open(const char *path, struct fuse_file_info *fi);
int hello_read(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi);


#endif /* SRC_OSADA_H_ */
