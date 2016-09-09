/*
 * errors.c
 *
 *  Created on: 7 sep. 2016
 *      Author: maximilianofelice
 */
#include "errors.h"

int _handle(char* str, int err, int silent) {
	if (err != 0) perror(str);
	if (silent) errno_clear;
	return err;
}

int _handle_fatal(char* str, int no){
	int err = _handle(str, no, 0);
	if (err != 0) exit(err);
	else return err;
}
