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

#define STRINGIFY(x)	#x
#define MACRO(x)    STRINGIFY(x)

/* Common operations */
DIR *open_dir(){
	return opendir(MACRO(MOUNTPOINT));
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
	while((ent = readdir(dir)) != NULL)
		if(strcmp(ent->d_name, dir_name) == 0) {
			closedir(dir);
			return 1;
		}
	closedir(dir);
	return 0;
}

/**
 * 	Arguments should be distinct!!!
 */
int dir_match(int num, ...){
	va_list args;
	va_start(args, num);

	int count = 0;
	while(dir_contains(va_arg(args, char*)) && count < num)
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

int rm_file(char* dir_name){
	char* dup = strdup(MACRO(MOUNTPOINT));
	int length = strlen(dup) + strlen(dir_name) + 1;
	char* dir = malloc(length);

	memset(dir, 0, length);
	memcpy(dir, dup, strlen(dup));
	memcpy(dir + strlen(dup), dir_name, strlen(dir_name));

	int res = remove(dir);

	free(dup);
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

		it("should be able to host up to MAXDIRENTRIES folders or files"){
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        	generate_names(MAXDIRENTRIES, mk_dir);
        	should_int(dir_count()) be equal to(MAXDIRENTRIES + 2);
        	generate_names(MAXDIRENTRIES, rm_file);
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        } end

		// TODO: Should the '\0' be counted as a char?
		it("should be able create a file with FILENAMEMAXLENGTH chars"){
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        	char *name = malloc(FILENAMEMAXLENGTH);
        	memset(name, 'a', FILENAMEMAXLENGTH);
        	name[FILENAMEMAXLENGTH - 1] = '\0';
        	errno = 0;
        	int res = mk_dir(name);
        	should_int(res) be equal to(0);
        	should_int(errno) be equal to(0);
        	rm_file(name);
        	free(name);
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        } end

		it("should NOT be able create a file with more than FILENAMEMAXLENGTH chars"){
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        	char *name = malloc(FILENAMEMAXLENGTH + 1);
        	memset(name, 'b', FILENAMEMAXLENGTH + 1);
        	name[FILENAMEMAXLENGTH] = '\0';
        	errno = 0;
        	int res = mk_dir(name);
        	should_int(res) be equal to(-1);
        	should_int(errno) be equal to(ENAMETOOLONG);
        	usleep(30000); // This is because the FS is not syncd
        	free(name);
        	should_bool(dir_match(2, ".", "..")) be equal to(true);
        } end

		it("should NOT be able to host more than MAXDIRENTRIES folders or files"){
			should_bool(dir_match(2, ".", "..")) be equal to(true);
			generate_names(MAXDIRENTRIES, mk_dir);
			int res = mk_dir("extra");
			should_int(res) be equal to(-1);
			should_int(errno) be equal to(EDQUOT);
			generate_names(MAXDIRENTRIES, rm_file);
			should_bool(dir_match(2, ".", "..")) be equal to(true);
		} end
    } end

}
