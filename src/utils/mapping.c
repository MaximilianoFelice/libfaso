/*
 * mapping.c
 *
 *  Created on: 7 sep. 2016
 *      Author: maximilianofelice
 */

#include "mapping.h"

/* Allocating resources */
static Disk* init_disk(char* head, uint64_t size, uint64_t block_size) {
	Disk* disk = malloc(sizeof(Disk));

	*(char**)&disk->head = head;
	*(uint64_t*)&disk->size = size;
	*(uint64_t*)&disk->block_size = block_size;
	*(uint64_t*)&disk->block_count = size / block_size;
	*(DiskZone**)&disk->zones = NULL;

	return disk;
}

static DiskZone* last_zone(DiskZone* zone){
	if (zone == NULL || zone->next == NULL) return zone;
	else return last_zone(zone->next);
}

static uint64_t zone_offset_acc(DiskZone* zone, uint64_t acc){
	if (zone == NULL || zone->next == NULL) return acc;
	else return zone_offset_acc(zone->next, acc + zone->size);
}

static uint64_t zone_offset(Disk* disk){
	return zone_offset_acc(disk->zones, 0);
}

static DiskZone* init_zone(Disk* disk, zone_attr attr, uint64_t blocks){

	DiskZone* zone = malloc(sizeof(DiskZone));
	uint64_t offset = zone_offset(disk);

	*(zone_attr*)&zone->attr = attr;
	*(uint64_t*)&zone->size = blocks;
	*(uint64_t*)&zone->offset = offset;
	*(DiskZone**)&zone->next = NULL;

	if (disk->zones == NULL)
		disk->zones = zone;
	else
		last_zone(disk->zones)->next = zone;
	return zone;
}

/* Freeing Resources */

void free_zone(DiskZone* zone){
	DiskZone* next = zone->next;
	if (next == NULL) free(zone);
	else {
		free(zone);
		free_zone(next);
	}
}

void free_disk(Disk* disk){
	munmap((char*) disk->head, disk->size);
	free_zone(disk->zones);
	free(disk);
}

/* Resources Operations */

uint64_t size(int fd){
	struct stat sbuf;
	if (fstat(fd, &sbuf) < 0) return errno;
	return sbuf.st_size;
}

void operate_disk(Disk* disk){
	madvise((char*) disk->head, disk->size * disk->block_size, MADV_RANDOM);
}

void operate_flags(Disk* disk, DiskZone* zone){
	zone_attr att = zone->attr;
	char* zone_start = (char*) (disk->head + (zone->offset * disk->block_size));

	if (att & (1 << IMPORTANT))
		mlock(zone_start, zone->size * disk->block_size);
}

Disk* open_disk(int fd, uint64_t block_size){
	uint64_t d_size = size(fd);
	char* disk = mmap64(NULL, d_size , PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, fd, 0);;

	Disk* new_disk = init_disk(disk, d_size, block_size);
	operate_disk(new_disk);

	return new_disk;
}

DiskZone* add_disk_zone(Disk* disk, uint64_t size, zone_attr attrs){
	DiskZone* new_zone = init_zone(disk, attrs, size);
	operate_flags(disk, new_zone);
	return new_zone;
}
