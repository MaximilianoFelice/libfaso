#include "osada.h"

osada_header* header(Disk* disk){
	return (osada_header*) disk;
}

int alloc_table_size(Disk* disk){
	osada_header* osada_header = header(disk);
	int alloc = osada_header->allocations_table_offset;
	int data = osada_header->data_blocks;
	int blocks = osada_header->fs_blocks;

	return blocks - (alloc + data);
}

Disk* disk = NULL;
void open_osada(){
	int disk_descriptor = open64(runtime_options.define_disc_path, O_RDWR, 0);
	handle_fatal("Error at opening file");

	disk = open_disk(disk_descriptor, OSADA_BLOCK_SIZE);
	add_disk_zone(disk, sizeof(osada_header), IMPORTANT);
	add_disk_zone(disk, header(disk)->bitmap_blocks, IMPORTANT);
	add_disk_zone(disk, OSADA_FILE_TABLE_BLOCKS, IMPORTANT);
	add_disk_zone(disk, alloc_table_size(disk), IMPORTANT);
	add_disk_zone(disk, header(disk)->data_blocks, NORMAL);
}

void load_zones(){
	// char* table = (char*) get(disk, FILE_TABLE);
	// zones.file_table = create_iterator(table, ARRAY(OSADA_FILE_TABLE_ENTRIES));
}

struct fuse_operations hello_oper =  {
	.getattr	= osada_getattr,
	.readdir	= osada_readdir,
	.mkdir		= osada_mkdir,
	.open		= hello_open,
	.read		= hello_read,
};

int main(int argc, char *argv[])
{
	/* Handling Options */
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	set_fuse_options(&args);
	handle_fatal("Error at parsing parameters");

	/* Opening Disk */
	open_osada();
	load_zones();

	return fuse_main(args.argc, args.argv, &hello_oper, NULL);
}

void unmount(){
	free_disk(disk);
	// free_iterator(zones.file_table);
}
