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
#include <libgen.h>
#include "utils/mapping.h"
#include "utils/errors.h"
#include "../osada-tools/osada.h"
#include "utils/fuseopts.h"
#include "collections/iterable.h"

/*
 * Este es el path de nuestro, relativo al punto de montaje, archivo dentro del FS
 */
#define DEFAULT_FILE_PATH "/" DEFAULT_FILE_NAME

#define OSADA_FILE_TABLE_BLOCKS 1024
#define OSADA_FILE_TABLE_ENTRIES (OSADA_FILE_TABLE_BLOCKS*2)


typedef struct OsadaZones {
	iterable* file_table;
} OsadaZones;

extern Disk* disk;
extern OsadaZones zones;

enum {
	DISK_HEADER,
	DISK_BITMAP,
	DISK_FILE_TABLE,
	DISK_ALLOC_TABLE,
	DISK_DATA
};

#define ROOT 0xFFFFFFFF
#define NO_FILE 0xFFFF
#define ROOT_DIR 0xFFFF

#define HEADER ((osada_header*) get(disk, DISK_HEADER))
#define BITMAP ((char*) get(disk, DISK_BITMAP))
#define FILE_TABLE ((osada_file*) get(disk, DISK_FILE_TABLE))
#define ALLOC_TABLE ((uint32_t*) get(disk, DISK_ALLOC_TABLE))
#define DATA ((osada_block*) get(disk, DISK_DATA))

/* FUSE Operations */
int osada_getattr(const char *path, struct stat *stbuf);
int osada_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi);
int hello_open(const char *path, struct fuse_file_info *fi);
int hello_read(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi);
int osada_mkdir(const char* path, mode_t mode);


#endif /* SRC_OSADA_H_ */
