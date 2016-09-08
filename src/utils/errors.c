/*
 * errors.c
 *
 *  Created on: 7 sep. 2016
 *      Author: maximilianofelice
 */
#include "errors.h"

int handle(char* str) {
	int err = errno;
	if (err < 0) perror(str);
	return err;
}

int handle_fatal(char* str){
	int err = handle(str);
	if (err < 0) exit(err);
	else return err;
}
