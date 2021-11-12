CC=gcc
CFLAGS=-I/usr/include/libpng -Wall -ggdb -Wextra -O2 -flto
LIBS=-lpng
LDFLAGS=-flto
VERSION=20211104
DISTDIR=${OUT}-${VERSION}
prefix=/usr/local

png2gds : png2gds.o gds.o readpng.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

png2gds.o : png2gds.c
	$(CC) -DVERSION=\"$(VERSION)\" $(CFLAGS) -c $< -o $@

gds.o : gds.c gds.h
	$(CC) -DVERSION=\"$(VERSION)\" $(CFLAGS) -c $< -o $@

readpng.o : readpng.c readpng.h
	$(CC) -DVERSION=\"$(VERSION)\" $(CFLAGS) -c $< -o $@

install: png2gds
	install -d "${DESTDIR}$(prefix)/bin"
	install -s png2gds ${DESTDIR}${prefix}/bin/png2gds
	install -d "${DESTDIR}$(prefix)/share/man/man1"
	install -m644 png2gds.1 ${DESTDIR}${prefix}/share/man/man1/png2gds.1

dist: png2gds
	rm -rf ${DISTDIR}
	mkdir ${DISTDIR}
	cp png2gds.c ${DISTDIR}
	cp gds.c ${DISTDIR}
	cp gds.h ${DISTDIR}
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
