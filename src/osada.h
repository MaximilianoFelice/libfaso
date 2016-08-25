/*
 * osada.h
 *
 *  Created on: 24 ago. 2016
 *      Author: maximilianofelice
 */

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <commons/log.h>
#include <stddef.h>

#ifndef SRC_OSADA_H_
#define SRC_OSADA_H_


struct t_runtime_options {
	char* welcome_msg;
	char* define_disc_path;
	char* log_level_param;
	char* log_path_param;
} runtime_options;

/** keys for FUSE_OPT_ options */
enum {
	KEY_VERSION,
	KEY_HELP,
};

/*
 * Este es el path de nuestro, relativo al punto de montaje, archivo dentro del FS
 */
#define DEFAULT_FILE_PATH "/" DEFAULT_FILE_NAME
#define CUSTOM_FUSE_OPT_KEY(t, p, v) { t, offsetof(struct t_runtime_options, p), v }


// Se guardara aqui la ruta al disco. Tiene un tamanio maximo.
char fuse_disc_path[1000];

// Define los datos del log
#define LOG_PATH fuse_log_path
char fuse_log_path[1000];

#endif /* SRC_OSADA_H_ */
