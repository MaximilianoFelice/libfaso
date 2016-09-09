/*
 * diropts.c
 *
 *  Created on: 7 sep. 2016
 *      Author: maximilianofelice
 */
#include "../osada.h"

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

int hello_getattr(const char *path, struct stat *stbuf)
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

int is_file(osada_file* file){
	return (file->state == REGULAR || file->state == DIRECTORY);
}

int compare(osada_file* file, char* token, uint16_t root){
	return (is_file(file) && strcmp((char*) file->fname, token) && (file->parent_directory == root));
}

/** CHECK ENOENT && UINT16: ITS UNSIGNED!!!! **/
uint16_t block_for_token(osada_file* table, uint16_t offset, char* token, uint16_t root){
	if (offset > OSADA_FILE_TABLE_ENTRIES) return -ENOENT;
	else if (compare(table, token, root)) return offset;
	else return block_for_token(table + 1, offset +1, token, root);
}

uint16_t block_for_path(char* path, uint16_t root){
	char* tok = strsep(&path, "/");
	if (tok == NULL) return block_for_token(FILE_TABLE, 0, path, root);
	else if(strcmp(tok, "")) return root;
	else return block_for_path(path, block_for_token(FILE_TABLE, 0, tok, root));
}

uint16_t get_block(const char* path){
	char *freeable, *path_copy;
	freeable = path_copy = strdup(path);
	uint16_t parent = block_for_path(path_copy, 0);
	free(freeable);
	return parent;
}

int osada_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

	uint16_t parent = get_block(path);
	if (parent < 0) return parent;

	for(int i = 0; i < OSADA_FILE_TABLE_ENTRIES; i++){
		if (is_file(FILE_TABLE + i) && FILE_TABLE[i].parent_directory == parent){
			filler(buf, (char*) FILE_TABLE[i].fname, NULL, 0);
		}
	}

	return 0;
}

int get_free_directory(osada_file* table, uint16_t offset){
	if (offset > OSADA_FILE_TABLE_ENTRIES) return -ENOENT;
	else if (!is_file(table)) return offset;
	else return get_free_directory(table + 1, offset + 1);
}

int osada_mkdir(const char* path, mode_t mode){
	// TODO: Dup interface in block_for_path.
	uint16_t parent = get_block(path);
	if (parent < 0) return parent;

	uint16_t block = get_free_directory(FILE_TABLE, 0);
	if (block < 0) return block;
	osada_file* dir = FILE_TABLE + block;

	dir->first_block = ROOT;
	dir->file_size = 0x0;
	char *fre, *base = strdup(path);
	memcpy(&(dir->fname), basename(base), strlen(base));
	free(fre);
	dir->parent_directory = parent;
	dir->lastmod = time(NULL);
	dir->state = DIRECTORY;

	return 0;

}

int hello_open(const char *path, struct fuse_file_info *fi)
{
	if (strcmp(path, hello_path) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}
