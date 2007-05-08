COMPILE=gcc -Wall -ggdb
INCLUDES=-I/usr/include/libpng12
LIBS=-lpng12
OUT=png2gds
DISTDIR=${OUT}_`date +%Y%m%d`

${OUT} : png2gds.c
	$(COMPILE) $(INCLUDES) $(LIBS) png2gds.c -o ${OUT}

install: ${OUT}
	install -s ${OUT} /usr/local/bin

dist: ${OUT}
	rm -rf ${DISTDIR}/
	mkdir ${DISTDIR}/
	mkdir ${DISTDIR}/cadence/
	cp *.c *.h ${DISTDIR}/
	cp COPYING ${DISTDIR}/
	cp Makefile ${DISTDIR}/
	tar -jcf ${DISTDIR}.tar.bz2 ${DISTDIR}

clean:
	rm -f $(OUT)
	rm -f *.o

test: ${OUT}
	./$(OUT) -i example.txt -o example.png -l ./layers.txt -p ./palette.txt -m 800

memtest: $(OUT)
	valgrind -v --leak-check=full --show-reachable=yes ./$(OUT) -i example.txt -o example.png -l ./layers.txt -p ./palette.txt
