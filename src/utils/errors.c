/*
 * errors.c
 *
 *  Created on: 7 sep. 2016
 *      Author: maximilianofelice
 */
#include "errors.h"

void _clear(){
	errno = 0;
}

int _handle(char* str, int silent) {
	int err = errno;
	if (err < 0) perror(str);
	if (silent) _clear();
	return err;
}

int _handle_fatal(char* str){
	int err = handle(str);
	if (err < 0) exit(err);
	else return err;
}
