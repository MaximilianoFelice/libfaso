#include "osada.h"


static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

static int hello_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, hello_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(hello_str);
	} else
		res = -ENOENT;

	return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, hello_path + 1, NULL, 0);

	return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
	if (strcmp(path, hello_path) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
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

static struct fuse_operations hello_oper = {
	.getattr	= hello_getattr,
	.readdir	= hello_readdir,
	.open		= hello_open,
	.read		= hello_read,
};

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

int main(int argc, char *argv[])
{
	/* Handling Options */
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	set_fuse_options(&args);
	handle_fatal("Error at parsing parameters");


	/* Opening Disk */
	open_disk();

	return fuse_main(args.argc, args.argv, &hello_oper, NULL);
}

void open_disk(){
	int disk_descriptor = open64(runtime_options.define_disc_path, O_RDWR, 0);
	int disk_descriptor = open64(runtime_options.define_disc_path, O_RDWR, 0);
	handle_fatal("Error at opening file");

	disk = open_disk(disk_descriptor, OSADA_BLOCK_SIZE);
	add_disk_zone(disk, sizeof(osada_header), IMPORTANT);
	add_disk_zone(disk, header(disk)->bitmap_blocks, IMPORTANT);
	add_disk_zone(disk, OSADA_FILE_TABLE_BLOCKS, IMPORTANT);
	add_disk_zone(disk, alloc_table_size(disk), IMPORTANT);
	add_disk_zone(disk, header(disk)->data_blocks, NORMAL);
}

void unmount(){
	free_disk(disk);
}
