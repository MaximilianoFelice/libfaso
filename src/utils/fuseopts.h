/*
 * fuseopts.h
 *
 *  Created on: 24 ago. 2016
 *      Author: maximilianofelice
 */

#ifndef SRC_UTILS_FUSEOPTS_H_
#define SRC_UTILS_FUSEOPTS_H_

#include <fuse.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>

#define CUSTOM_FUSE_OPT_KEY(t, p, v) { t, offsetof(struct t_runtime_options, p), v }

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

extern t_log_level log_level;

/*
 * Esta estructura es utilizada para decirle a la biblioteca de FUSE que
 * parametro puede recibir y donde tiene que guardar el valor de estos
 */
const static struct fuse_opt fuse_options[] = {

		// Si se le manda el parametro "--Disc-Path", lo utiliza:
		CUSTOM_FUSE_OPT_KEY("--Disc-Path=%s", define_disc_path, 0),

		// Define el log level
		CUSTOM_FUSE_OPT_KEY("--ll=%s", log_level_param, 0),

		// Define el log path
		CUSTOM_FUSE_OPT_KEY("--Log-Path", log_path_param, 0),

		// Estos son parametros por defecto que ya tiene FUSE
		FUSE_OPT_KEY("-V", KEY_VERSION),
		FUSE_OPT_KEY("--version", KEY_VERSION),
		FUSE_OPT_KEY("-h", KEY_HELP),
		FUSE_OPT_KEY("--help", KEY_HELP),
		FUSE_OPT_END,
};

// Se guardara aqui la ruta al disco. Tiene un tamanio maximo.
char fuse_disc_path[1000];

// Define los datos del log
#define LOG_PATH fuse_log_path
char fuse_log_path[1000];

// Exported Functions
int set_fuse_options(struct fuse_args *args);

#endif /* SRC_UTILS_FUSEOPTS_H_ */
