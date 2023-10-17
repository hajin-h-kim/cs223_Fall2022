CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g3

all: Unit Heatmap

Unit: track_unit.o track.o trackpoint.o location.o
	${CC} ${CFLAGS} -o $@ $^ -lm

Heatmap: heatmap.o track.o trackpoint.o location.o
	${CC} ${CFLAGS} -o $@  $^ -lm

clean:
	rm *.o Unit Heatmap

heatmap.o: heatmap.c
track_unit.o: track_unit.c
track.o: track.c
trackpoint.o: trackpoint.c
location.o: location.c