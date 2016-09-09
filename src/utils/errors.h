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

int _handle_fatal(char*, int err);
int _handle(char*, int no, int silent);
void _clear();

#define errno_ret2(val, sth) \
	if (val < 0) return sth

#define errno_ret1(sth) errno_ret2(errno, sth)

#define errno_ret(x, ...) errno_ret1(x)

#define errno_clear \
	errno = 0

#define handle_fatal(msg) _handle_fatal(msg, errno)
#define handle(msg) _handle(msg, errno, 0)
#define handle_return(msg) \
	errno_ret2(errno, handle(msg))

#define handle_silent(msg) _handle(msg, errno, 1)
#define handle_return_silent(msg) \
	errno_ret(handle_silent(msg));

#endif /* SRC_UTILS_ERRORS_H_ */
