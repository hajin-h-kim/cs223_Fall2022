#ifndef __TREE_HELPERS_H__
#define __TREE_HELPERS_H__

typedef struct _node{
    int key;
    struct _node* left;
    struct _node* right;
} node;

void print2DUtil(node* root, int space);
#endif