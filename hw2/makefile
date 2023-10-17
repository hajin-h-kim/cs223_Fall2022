CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g
TSP: hw2.o location.o
	${CC} -o $@ ${CFLAGS} $^ -lm

hw2.o: hw2.c location.h

location.o: location.c location.h
