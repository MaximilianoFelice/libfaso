# Compiler
CC=gcc
CFLAGS=-g -Wall
DEFS=-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
LIBS=-lfuse -lcommons

# Folders
RELEASE=release

all: osada-fs

$(RELEASE)/errors.o: src/utils/errors.c src/utils/errors.h
	$(CC) $(CFLAGS) $(DEFS) -c src/utils/errors.c -o $(RELEASE)/errors.o

$(RELEASE)/mapping.o: src/utils/mapping.c src/utils/mapping.h
	$(CC) $(CFLAGS) $(DEFS) -c src/utils/mapping.c -o $(RELEASE)/mapping.o
	
$(RELEASE)/fuseopts.o: src/utils/fuseopts.c src/utils/fuseopts.h
	$(CC) $(CFLAGS) $(DEFS) -c src/utils/fuseopts.c -o $(RELEASE)/fuseopts.o
	
$(RELEASE)/commons.o: src/fuseops/commons.c src/fuseops/commons.h
	$(CC) $(CFLAGS) $(DEFS) -c src/fuseops/commons.c -o $(RELEASE)/commons.o
	
$(RELEASE)/ioops.o: src/fuseops/ioops.c src/fuseops/commons.h
	$(CC) $(CFLAGS) $(DEFS) -c src/fuseops/ioops.c -o $(RELEASE)/ioops.o

$(RELEASE)/dirops.o: src/fuseops/dirops.c src/fuseops/commons.h src/osada.h
	$(CC) $(CFLAGS) $(DEFS) -c src/fuseops/dirops.c -o $(RELEASE)/dirops.o
	
$(RELEASE)/osada.o: src/osada.c src/osada.h
	$(CC) $(CFLAGS) $(DEFS) -c src/osada.c -o $(RELEASE)/osada.o

osada-fs: 	$(RELEASE)/errors.o 	\
			$(RELEASE)/mapping.o 	\
			$(RELEASE)/fuseopts.o 	\
			$(RELEASE)/commons.o 	\
			$(RELEASE)/ioops.o 		\
			$(RELEASE)/dirops.o		\
			$(RELEASE)/osada.o
	$(CC) $(CFLAGS) $(DEFS) -o $(RELEASE)/osada-fs $^ $(LIBS)
			

test: osada-fs
	$(CC) $(CFLAGS) $(DEFS) -o $(RELEASE)/test tests/dirspec.c $(LIBS) -lcspecs
	./release/test
	
clean:
	$(RM) $(RELEASE)/*.o $(RELEASE)/osada-fs