/*
 * dirspec.c
 *
 *  Created on: 13 sep. 2016
 *      Author: maximilianofelice
 */

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <cspecs/cspec.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "fs-defs.h"

#define STRINGIFY(x)	#x
#define MACRO(x)    STRINGIFY(x)

const char *mountpoint = MACRO(_MOUNTPOINT);
const int max_dir_entries = _MAX_DIR_ENTRIES;
const int max_file_name_length = _FILE_NAME_MAX_LENGTH;
size_t max_file_size = _FILE_MAX_SIZE;
const int block_size = _BLOCK_SIZE;

/* Common operations */
DIR *open_dir(){
	return opendir(mountpoint);
}

int dir_count(){
	int count;
	DIR *dir = open_dir();
	for(count = 0; readdir(dir) != NULL; count++);
	closedir(dir);
	return count;
}

int dir_contains(char* dir_name){
	DIR* dir = open_dir();
	struct dirent *ent;
	while((ent = readdir(dir)) != NULL && dir_name != NULL)
		if(strcmp(ent->d_name, dir_name) == 0) {
			closedir(dir);
			return 1;
		}
	closedir(dir);
	return 0;
}

/**
 * 	Should be freed!!!
 */
char *with_basename(const char* path){
	char* dup = strdup(mountpoint);
	int length = strlen(dup) + strlen(path) + 1;
	char* res = malloc(length);

	memset(res, 0, length);
	memcpy(res, dup, strlen(dup));
	memcpy(res + strlen(dup), path, strlen(path));

	free(dup);
	return res;
}

/**
 * 	Arguments should be distinct!!!
 */
int dir_match(int num, ...){
	va_list args;
	va_start(args, num);

	int count = 0;
	while(count < num && dir_contains(va_arg(args, char*)))
		count++;
	return (count == num) && (count == dir_count());
}

int mk_dir(char* dir_name){
	DIR* dir = open_dir();
	int fd = dirfd(dir);
	int res = mkdirat(fd, dir_name, 0777);
	closedir(dir);
	return res;
}

int mk_file(char* file_name){
	DIR* dir = open_dir();
	int fd = dirfd(dir);
	int res = mknodat(fd, file_name, 0777, 0);
	closedir(dir);
	return res;
}

int rm_file(char* dir_name){
	char* dir = with_basename(dir_name);
	int res = remove(dir);
	free(dir);
	return res;
}

void generate_names(int count, int(operator)(char*)){
	char *str = malloc(count);
	for(int i = 0; i < count; i++) {
		sprintf(str, "%d", i);
		operator(str);
	}
	free(str);
}

int write_to_file(const char* file, char *buffer, size_t size){
	char *path = with_basename(file);
	int fd = open(path, O_RDWR);
	int res = write(fd, buffer, size);
	close(fd);
	free(path);
	return res;
}

int write_random_data(const char* file, size_t size){
	char *buff = malloc(size);
	memset(buff, 'c', size);
	int res = write_to_file(file, buff, size);
	free(buff);
	return res;
}

size_t file_size(const char *file){
	char *path = with_basename(file);
	struct stat *stat = malloc(sizeof(struct stat));
	int fd = open(path, O_RDWR);
	fstat(fd, stat);
	close(fd);
	free(path);
	return stat->st_size;
}

context (dirspec) {

    describe("Directory Operations") {
        it("should read mountpoint folders") {
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        } end

		it("should be able to create new folders"){
        	mk_dir("dirent");
        	should_bool(dir_match(3, ".", "..", "dirent")) be equal to(true);
        } end

		it("should be able to delete folders"){
        	rm_file("dirent");
        	usleep(30000); // This is because the FS is not syncd
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        } end

		it("should be able to host up to max_dir_entries folders or files"){
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        	generate_names(max_dir_entries, mk_dir);
        	should_int(dir_count()) be equal to(max_dir_entries + 2);
        	generate_names(max_dir_entries, rm_file);
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        } end

		// TODO: Should the '\0' be counted as a char?
		it("should be able create a file with max_file_name_length chars"){
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        	char *name = malloc(max_file_name_length);
        	memset(name, 'a', max_file_name_length);
        	name[max_file_name_length - 1] = '\0';
        	errno = 0;
        	int res = mk_dir(name);
        	should_int(res) be equal to(0);
        	should_int(errno) be equal to(0);
        	rm_file(name);
        	free(name);
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        } end

		it("should NOT be able create a file with more than max_file_name_length chars"){
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        	char *name = malloc(max_file_name_length + 1);
        	memset(name, 'b', max_file_name_length + 1);
        	name[max_file_name_length] = '\0';
        	errno = 0;
        	int res = mk_dir(name);
        	should_int(res) be equal to(-1);
        	should_int(errno) be equal to(ENAMETOOLONG);
        	free(name);
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        } end

		it("should create a file on folder"){
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        	errno = 0;
        	mk_file("file");
        	should_int(errno) be equal to(0);
        	should_bool(dir_match(3, ".", "..", "file")) be equal to(true);
        	rm_file("file");
        	should_int(errno) be equal to(0);
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        } end

		it("should NOT be able to host more than max_dir_entries folders or files"){
			should_bool(dir_match(2, ".", "..")) be equal to(true);
			generate_names(max_dir_entries, mk_dir);
			int res = mk_dir("extra");
			should_int(res) be equal to(-1);
			should_int(errno) be equal to(EDQUOT);
			generate_names(max_dir_entries, rm_file);
			should_bool(dir_match(2, ".", "..")) be equal to(true);
		} end

		it("should be able to host a file of size MAXFILESIZE"){
			should_bool(dir_match(2, ".", "..")) be equal to(true);
			mk_file("fat");
			should_bool(dir_match(3, ".", "..", "fat")) be equal to(true);
			errno = 0;
			write_random_data("fat", max_file_size);
			should_int(errno) be equal to(0);
			should_bool(dir_match(3, ".", "..", "fat")) be equal to(true);
			should_int(file_size("fat")) be equal to(max_file_size);
			rm_file("fat");
			should_bool(dir_match(2, ".", "..")) be equal to(true);
		} end

		/*
		it("should FAIL to host a file of size MAXFILESIZE + 1"){
			should_bool(dir_match(2, ".", "..")) be equal to(true);
			mk_file("fat");
			errno = 0;
			int res = write_random_data("fat", max_file_size + 1);
			usleep(3000);
			should_int(res) be equal to(-1);
			should_int(errno) be equal to(EDQUOT);
			usleep(3000);
			rm_file("fat");
			should_bool(dir_match(2, ".", "..")) be equal to(true);
		} end
		*/
    } end

}
