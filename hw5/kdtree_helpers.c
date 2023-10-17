#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "location.h"
#include "kdtree_helpers.h"

// auxiliary function for kdtree_create()
// used as a comparator function for qsort
int compare_latitude(const void* loc1, const void* loc2)
{
    location l1 = *(location*)loc1;
    location l2 = *(location*)loc2;
    if (l1.lat < l2.lat)
    {
        return -1;
    }
    else if (l1.lat > l2.lat)
    {
        return 1;
    }
    else if (l1.lon < l2.lon)
    {
        return -1;
    }
    else if (l1.lon > l2.lon)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// auxiliary function for kdtree_create()
// used as a comparator function for qsort
int compare_longitude(const void* loc1, const void* loc2)
{
    location l1 = *(location*)loc1;
    location l2 = *(location*)loc2;
    if (l1.lon < l2.lon)
    {
      return -1;
    }
    else if (l1.lon > l2.lon)
    {
      return 1;
    }
    else if (l1.lat < l2.lat)
    {
      return -1;
    }
    else if (l1.lat > l2.lat)
    {
      return 1;
    }
    else
    {
      return 0;
    }
}

// auxiliary function for internal_create()
// determines if a location should go to the cut_left array for the next recursion
bool in_cut_left(location this, location median, int d)
{
    if(d == 0)
        if(location_compare_latitude(&this, &median) < 0) return true;

    if(d == 1)
        if(location_compare_longitude(&this, &median) < 0) return true;

    return false;
}

// auxiliary function for internal_create()
// determines if a location should go to the cut_right array for the next recursion
bool in_cut_right(location this, location median, int d)
{
    if(d == 0)
        if(location_compare_latitude(&this, &median) > 0) return true;

    if(d == 1)
        if(location_compare_longitude(&this, &median) > 0) return true;
    
    return false;
}

// auxiliary function for kdtree_remove()
// finds the minimum location (by the cutting dimension) in the subtree
node* find_min(node* root, int dim_cut, int depth)
{
    if(root == NULL) return NULL;

    int dim = depth % K;
    if(dim == dim_cut)
    {
        if(root->left != NULL)
            return find_min(root->left, dim_cut, depth+1);
        else
            return root;
    }
    else
    {
        return min_of_three(root, find_min(root->left, dim_cut, depth+1),
                                  find_min(root->right, dim_cut, depth+1), dim_cut);
    }
}

// auxiliary function for kdtree_remove()
// finds the maximum location (by the cutting dimension) in the subtree
node* find_max(node* root, int dim_cut, int depth)
{
    if(root == NULL) return NULL;

    int dim = depth % K;
    if(dim == dim_cut)
    {
        if(root->right != NULL)
            return find_max(root->right, dim_cut, depth+1);
        else
            return root;
    }
    else
    {
        return max_of_three(root, find_max(root->left, dim_cut, depth+1),
                                  find_max(root->right, dim_cut, depth+1), dim_cut);
    }
}

// auxiliary function for find_min() of kdtree_remove()
// finds the minimum among itself, its left child, and its right child
node* min_of_three(node* x, node* y, node* z, int dim_cut)
{
    // we know for sure x (root from find_min) is not NULL
    node* min = x;
    if(dim_cut == 0)
    {
        if(y!= NULL)
            if(location_compare_latitude(&(y->key), &(min->key)) < 0) min = y;

        if(z!= NULL)
            if(location_compare_latitude(&(z->key), &(min->key)) < 0) min = z;
    }
    else
    {
        if(y!= NULL)
            if(location_compare_longitude(&(y->key), &(min->key)) < 0) min = y;

        if(z!= NULL)
            if(location_compare_longitude(&(z->key), &(min->key)) < 0) min = z;
    }
    return min;
}

// auxiliary function for find_max() of kdtree_remove()
// finds the aximum among itself, its left child, and its right child
node* max_of_three(node* x, node* y, node* z, int dim_cut)
{
    // we know for sure x (root from find_min) is not NULL
    node* max = x;
    if(dim_cut == 0)
    {
        if(y!= NULL)
            if(location_compare_latitude(&(y->key), &(max->key)) > 0) max = y;

        if(z!= NULL)
            if(location_compare_latitude(&(z->key), &(max->key)) > 0) max = z;
    }
    else
    {
        if(y!= NULL)
            if(location_compare_longitude(&(y->key), &(max->key)) > 0) max = y;

        if(z!= NULL)
            if(location_compare_longitude(&(z->key), &(max->key)) > 0) max = z;
    }
    return max;
}

// auxiliary function for kdtree_range()
// adds a point (in the range) to the final array of location points
void add_to_arr(const location* loc, void* a)
{
    if(loc == NULL || a == NULL) return;

    Array* x = (Array*) a;
    x->count = x->count+1;
    x->array[x->count-1] = *loc;
    
    return;
}
