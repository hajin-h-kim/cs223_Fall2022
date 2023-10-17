CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g3

all: AVL RBT BST

AVL: avltree.o
	${CC} ${CFLAGS} -o $@ $^ -lm

RBT: rbtree.o treehelpers.o
	${CC} ${CFLAGS} -o $@ $^ -lm

BST: bstree.o treehelpers.o
	${CC} ${CFLAGS} -o $@ $^ -lm

clean:
	rm *.o AVL RBT BST

avltree.o: avltree.c
rbtree.o: rbtree.c
bstree.o: bstree.c
treehelpers.o: treehelpers.c
