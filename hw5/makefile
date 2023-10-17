CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g3

all: Unit

Unit: kdtree_unit.o kdtree.o location.o kdtree_helpers.o
	${CC} ${CFLAGS} -o $@ $^ -lm

clean:
	rm *.o Unit

kdtree.o: kdtree.c
kdtree_unit.o: kdtree_unit.c
location.o: location.c
kdtree_helpers.o: kdtree_helpers.c