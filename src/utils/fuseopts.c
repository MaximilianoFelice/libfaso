/*
 * fuseopts.c
 *
 *  Created on: 24 ago. 2016
 *      Author: maximilianofelice
 */

#include "fuseopts.h"

int set_fuse_options(struct fuse_args *args){
	// Limpio la estructura que va a contener los parametros
	memset(&runtime_options, 0, sizeof(struct t_runtime_options));

	// Esta funcion de FUSE lee los parametros recibidos y los intepreta
	if (fuse_opt_parse(args, &runtime_options, fuse_options, NULL) == -1){
		/** error parsing options */
		perror("Invalid arguments!");
		return EXIT_FAILURE;
	}

	// Setea el path del disco
	if (runtime_options.define_disc_path != NULL){
		strcpy(fuse_disc_path, runtime_options.define_disc_path);
	} else{
		printf("Mountpoint not specified: Unloading modules.");
		exit(0);
	}

	// Settea el log level del disco:
	t_log_level log_level = LOG_LEVEL_ERROR;
	if (runtime_options.log_level_param != NULL){
		if (!strcmp(runtime_options.log_level_param, "Trace")) log_level = LOG_LEVEL_TRACE;
		else if (!strcmp(runtime_options.log_level_param, "Debug")) log_level = LOG_LEVEL_DEBUG;
		else if (!strcmp(runtime_options.log_level_param, "Info")) log_level = LOG_LEVEL_INFO;
		else if (!strcmp(runtime_options.log_level_param, "Warning")) log_level = LOG_LEVEL_WARNING;
		else if (!strcmp(runtime_options.log_level_param, "Error")) log_level = LOG_LEVEL_ERROR;
	}

	// Settea el log path
	if (runtime_options.log_path_param != NULL){
		strcpy(fuse_log_path,runtime_options.log_path_param);
	} else {
		log_level = LOG_LEVEL_ERROR;
	}

	return EXIT_SUCCESS;
}
