COMPILE=gcc -Wall -ggdb
INCLUDES=-I/usr/include/libpng12
LIBS=-lpng12
OUT=png2gds
VERSION=20070807
DISTDIR=${OUT}_${VERSION}

${OUT} : png2gds.c
	$(COMPILE) -D$(VERSION) $(INCLUDES) $(LIBS) png2gds.c -o ${OUT}

install: ${OUT}
	install -s ${OUT} /usr/local/bin

dist: ${OUT}
	rm -rf ${DISTDIR}
	mkdir ${DISTDIR}
	cp *.c *.h ${DISTDIR}
	cp circ.png ${DISTDIR}
	cp COPYING ${DISTDIR}
	cp Makefile ${DISTDIR}
	tar -jcf ${DISTDIR}.tar.bz2 ${DISTDIR}

clean:
	rm -f $(OUT)
	rm -f *.o

test: ${OUT}
	./$(OUT) circ.png circ.gds 0.02

memtest: $(OUT)
	valgrind -v --leak-check=full --show-reachable=yes ./$(OUT) circ.png circ.gds 0.02
