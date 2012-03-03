CC=gcc
CFLAGS=-I/usr/include/libpng12 -Wall -ggdb -fno-common -Wall -Wdeclaration-after-statement -Wextra -Wformat=2 -Winit-self -Winline -Wpacked -Wp,-D_FORTIFY_SOURCE=2 -Wpointer-arith -Wlarger-than-65500 -Wmissing-declarations -Wmissing-format-attribute -Wmissing-noreturn -Wmissing-prototypes -Wnested-externs -Wold-style-definition -Wredundant-decls -Wsign-compare -Wstrict-aliasing=2 -Wstrict-prototypes -Wswitch-enum -Wundef -Wunreachable-code -Wwrite-strings
LIBS=-lpng12 -lm
LDFLAGS=
VERSION=20070807
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
	install -s png2gds ${DESTDIR}${prefix}/bin/png2gds

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
