/*
 * errors.h
 *
 *  Created on: 7 sep. 2016
 *      Author: maximilianofelice
 */

#ifndef SRC_UTILS_ERRORS_H_
#define SRC_UTILS_ERRORS_H_

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int _handle_fatal(char*);
int _handle(char*);

#define handle_fatal(msg) _handle_fatal(msg)
#define handle(msg) _handle(msg)

#endif /* SRC_UTILS_ERRORS_H_ */
