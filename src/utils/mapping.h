/*
 * mapping.h
 *
 *  Created on: 7 sep. 2016
 *      Author: maximilianofelice
 */

#ifndef SRC_UTILS_MAPPING_H_
#define SRC_UTILS_MAPPING_H_

#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

//#define TEST_FLAG(var, flag) (var & (1 << flag))
#define TEST_FLAG(var, flag) ((var & flag) != 0)
typedef uint32_t zone_attr;

typedef struct DiskZone {
	const zone_attr attr;
	const uint64_t offset;
	const char* start;
	const uint64_t size;
	struct DiskZone* next;
} DiskZone;

typedef struct {
	const char* head;
	const uint64_t size;
	const uint64_t block_size;
	const uint64_t block_count;
	DiskZone* zones;
} Disk;

uint64_t size(int fd);
Disk* open_disk(int fd, uint64_t block_size);
DiskZone* add_disk_zone(Disk* disk, uint64_t blocks, zone_attr attrs);
void free_disk(Disk* disk);
DiskZone* get_zone(Disk* disk, int zone);

#define	NORMAL 0x1
#define IMPORTANT 0x2

#endif /* SRC_UTILS_MAPPING_H_ */
