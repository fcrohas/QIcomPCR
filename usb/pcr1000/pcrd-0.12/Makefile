###############################################################################
#
#Makefile for 'pcrd' 
#
###############################################################################

CC=gcc
CFLAGS=

foo:
	@echo "Target(s): solaris linux freebsd debug_solaris debug_linux debug_freebsd clean"

freebsd: pcrd.c
	$(CC) $(CFLAGS) -o pcrd pcrd.c

debug_freebsd:    pcrd.c
	$(CC) $(CFLAGS) -ggdb -o pcrd pcrd.c

linux: pcrd.c
	$(CC) $(CFLAGS) -DLINUX -o pcrd pcrd.c

solaris: pcrd.c
	cc $(CFLAGS) -DSOLARIS -o pcrd pcrd.c -lsocket -lnsl

debug_linux:    pcrd.c
	$(CC) $(CFLAGS) -DLINUX -ggdb -o pcrd pcrd.c

debug_solaris:  pcrd.c
	cc $(CFLAGS) -DSOLARIS -g -o pcrd pcrd.c -lsocket -lnsl

all: pcrd.c
	$(CC) $(CFLAGS) -o pcrd pcrd.c

clean: 
	rm pcrd
