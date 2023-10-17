CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g3

all: GmapUnit Blotto

GmapUnit: gmap_unit.o gmap.o gmap_test_functions.o string_key.o
	${CC} ${CFLAGS} -o $@ $^ -lm

Blotto: blotto.o gmap.o entry.o string_util.o
	${CC} ${CFLAGS} -o $@ $^ -lm

clean:
	rm *.o GmapUnit Blotto

blotto.o: blotto.c
gmap.o: gmap.c
gmap_unit.o: gmap_unit.c
gmap_test_functions.o: gmap_test_functions.c
string_key.o: string_key.c
entry.o: entry.c
string_util.o: string_util.c