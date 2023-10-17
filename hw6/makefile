CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -g3

Paths: paths.o ldigraph.o ldigraph_helpers.o
	${CC} -o $@ ${CFLAGS} $^

ldigraph.o: ldigraph.c
ldigraph_helpers.o: ldigraph_helpers.c
paths.o: paths.c
