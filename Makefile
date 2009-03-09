CC=gcc
CFLAGS=-I/usr/include/libpng12 -Wall -ggdb
LIBS=-lpng12
LDFLAGS=
VERSION=20070807
DISTDIR=${OUT}-${VERSION}

png2gds : png2gds.c
	$(CC) -DVERSION=\"$(VERSION)\" $(CFLAGS) $(LIBS) $(LDFLAGS) $^ -o $@

install: png2gds
	install -s png2gds /usr/local/bin/png2gds

dist: png2gds
	rm -rf ${DISTDIR}
	mkdir ${DISTDIR}
	cp png2gds.c ${DISTDIR}
	cp circ.png ${DISTDIR}
	cp COPYING ${DISTDIR}
	cp Makefile ${DISTDIR}
	cp readme.txt ${DISTDIR}/
	tar -jcf ${DISTDIR}.tar.bz2 ${DISTDIR}

clean:
	rm -f png2gds
	rm -f *.o

test: png2gds
	./png2gds circ.png circ.gds 0.02

memtest: png2gds
	valgrind -v --leak-check=full --show-reachable=yes ./png2gds circ.png circ.gds 0.02
