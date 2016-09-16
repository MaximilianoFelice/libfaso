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

#define STRINGIFY(x) #x
#define MACRO(x)     STRINGIFY(x)

DIR *open_dir(){
	return opendir(MACRO(MOUNTPOINT));
}

int count_directories(){
	int count;
	DIR *dir = open_dir();
	for(count = 0; readdir(dir) != NULL; count++);
	return count;
}

int directories_contains(char* dir_name){
	DIR* dir = open_dir();
	struct dirent *ent;
	while((ent = readdir(dir)) != NULL)
		if(strcmp(ent->d_name, dir_name) == 0) return 1;
	return 0;
}

/**
 * 	Arguments should be distinct!!!
 */
int directories_match(int num, ...){
	va_list args;
	va_start(args, num);

	int count = 0;
	while(directories_contains(va_arg(args, char*)) && count < num)
		count++;
	return count == num;
}

int make_dir(char* dir_name){
	DIR* dir = open_dir();
	int fd = dirfd(dir);
	return mkdirat(fd, dir_name, 0777);
}

context (dirspec) {

    describe("Directory Operations") {
        it("should read mountpoint folders") {
        	should_bool(directories_match(2, ".", "..")) be equal to(true);
        } end

		it("should be able to create new folders"){
        	make_dir("dirent");
        	should_bool(directories_match(3, ".", "..", "dirent")) be equal to(true);
        } end
    } end

}
