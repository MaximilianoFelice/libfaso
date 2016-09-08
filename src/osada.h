/*
 * osada.h
 *
 *  Created on: 24 ago. 2016
 *      Author: maximilianofelice
 */

#ifndef SRC_OSADA_H_
#define SRC_OSADA_H_

#define FUSE_USE_VERSION 29

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "utils/fuseopts.h"
#include "utils/mapping.h"
#include "utils/errors.h"
#include "../osada-tools/osada.h"

/*
 * Este es el path de nuestro, relativo al punto de montaje, archivo dentro del FS
 */
#define DEFAULT_FILE_PATH "/" DEFAULT_FILE_NAME

#define OSADA_FILE_TABLE_BLOCKS 1024

Disk* disk;

#endif /* SRC_OSADA_H_ */
