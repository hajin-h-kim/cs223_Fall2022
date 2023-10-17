#ifndef __KDTREE_HELPERS_H__
#define __KDTREE_HELPERS_H__

typedef struct _node{
    location key;
    struct _node *left, *right;
} node;

typedef struct {
    int count;
    location* array;
} Array;

#define K 2

int compare_latitude(const void* loc1, const void* loc2);
int compare_longitude(const void* loc1, const void* loc2);
bool in_cut_left(location this, location median, int d);
bool in_cut_right(location this, location median, int d);
node* find_min(node* root, int dim_cut, int depth);
node* find_max(node* root, int dim_cut, int depth);
node* min_of_three(node* x, node* y, node* z, int dim_cut);
node* max_of_three(node* x, node* y, node* z, int dim_cut);
void add_to_arr(const location* loc, void* a);

#endif