/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall hello.c `pkg-config fuse --cflags --libs` -o hello
*/

#define FUSE_USE_VERSION 26

#include "osada.h"


/*
 * Esta estructura es utilizada para decirle a la biblioteca de FUSE que
 * parametro puede recibir y donde tiene que guardar el valor de estos
 */
static struct fuse_opt fuse_options[] = {

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

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

static int hello_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, hello_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(hello_str);
	} else
		res = -ENOENT;

	return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, hello_path + 1, NULL, 0);

	return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
	if (strcmp(path, hello_path) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;
	if(strcmp(path, hello_path) != 0)
		return -ENOENT;

	len = strlen(hello_str);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, hello_str + offset, size);
	} else
		size = 0;

	return size;
}

static struct fuse_operations hello_oper = {
	.getattr	= hello_getattr,
	.readdir	= hello_readdir,
	.open		= hello_open,
	.read		= hello_read,
};

int main(int argc, char *argv[])
{
	// Limpio la estructura que va a contener los parametros
	memset(&runtime_options, 0, sizeof(struct t_runtime_options));

	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	// Esta funcion de FUSE lee los parametros recibidos y los intepreta
	if (fuse_opt_parse(&args, &runtime_options, fuse_options, NULL) == -1){
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


	return fuse_main(args.argc, args.argv, &hello_oper, NULL);
}
