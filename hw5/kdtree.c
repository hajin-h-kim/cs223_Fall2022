#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "location.h"
#include "kdtree_helpers.h"
#include "kdtree.h"

struct _kdtree{
    node* root;
    int n;
};

// ==========================================================================
// Helper Functions
// ==========================================================================
node* internal_create(kdtree*t, location* cut, location* other, int n, int d);
node* internal_remove(node* par, node* curr, const location* p, int depth, bool* removed);
void internal_range_for_each(node* root, int depth, double e, double w, double n, double s, void (*f)(const location *, void *), void *arg);
void internal_destroy(node* curr);

// ==========================================================================
// ADT Function Implementation
// ==========================================================================
/**
 * Creates a set of points in a balanced k-d tree containing copies of
 * the points in the given array of locations.  If n is 0 then the
 * returned tree is empty.  If the array contains multiple copies of
 * the same point (with "same" defined as described above), then only
 * one copy is included in the set.
 *
 * @param pts an array of valid locations; NULL is allowed if n = 0
 * @param n the number of points to add from the beginning of that array,
 * or 0 if pts is NULL
 * @return a pointer to the newly created set of points
 */
kdtree *kdtree_create(const location *pts, int n)
{
    kdtree* t = malloc(1 * sizeof(kdtree));
    t->root = NULL;
    t->n = n;

    if(n == 0)
        return t;
    else{
        if(pts == NULL){
            free(t);
            return NULL;
        }

        location* x = malloc(n * sizeof(location));
        location* y = malloc(n * sizeof(location));
        for(int i = 0; i < n; i++)
        {
            x[i] = pts[i];
            y[i] = pts[i];
        }
        qsort(x, n, sizeof(location), compare_latitude);
        qsort(y, n, sizeof(location), compare_longitude);

        // cutting dimension
        int d = 0;
        t->root = internal_create(t, x, y, n, d);

        free(x);
        free(y);
        return t;
    }
}

node* internal_create(kdtree*t, location* cut, location* other, int n, int depth){
    // base case
    if (n == 0) return NULL;
    
    int dim = depth % K;
    int median = n/2;
    int n_left = median;
    int n_right = n-median-1;

    // printf("at depth %d: ", depth);
    // printf("median is %d, n_left %d, n_right %d.\n", median, n_left, n_right);

    node* newnode = malloc(sizeof(node));

    location cut_left[n_left];
    location cut_right[n_right];
    location other_left[n_left];
    location other_right[n_right];

    // fill cut_left/right. this is the easier half--just copy the values in the same order
    for(int i = 0; i < median ; i++)
        cut_left[i] = cut[i];
    for(int i = median + 1; i < n; i++)
        cut_right[i - median - 1] = cut[i];

    // fill other_left/right. parse through other[] & put each location in either left or right
    int ileft = 0;
    int iright = 0;
    for(int i = 0; i < n; ++i)
    {
        if(in_cut_left(other[i], cut[median], dim))
            other_left[ileft++] = other[i];

        else if(in_cut_right(other[i], cut[median], dim))
            other_right[iright++] = other[i];
    }
    
    newnode->key = cut[median];
    newnode->left = internal_create(t, other_left, cut_left, n_left, depth+1);
    newnode->right = internal_create(t, other_right, cut_right, n_right, depth+1);

    return newnode;
}


/**
 * Adds a copy of the given point to the given k-d tree.  There is no
 * effect if the point is already in the tree.  The tree need not be
 * balanced after the add.  The return value is true if the point was
 * added successfully and false otherwise (if the point was already in the
 * tree).
 *
 * @param t a pointer to a valid k-d tree, non-NULL
 * @param p a pointer to a valid location, non-NULL
 * @return true if and only if the point was successfully added
 */
bool kdtree_add(kdtree *t, const location *p)
{
    if(t == NULL || p == NULL){
        fprintf(stderr, "The tree or location pointer was invalid\n");
        return false;
    }

    int dim = 0;
    node* par = NULL;
    node* curr = t->root;

    bool t_is_empty = (t->root == NULL);

    while(curr != NULL)
    {
        par = curr;
        if(dim == 0) // compare lat
        {
            int comp = location_compare_latitude(p, &(curr->key));
            if(comp < 0)
                curr = curr->left;
            else if(comp > 0)
                curr = curr->right;
            else
                break;
        }
        if(dim == 1) // compare lon
        {
            int comp = location_compare_longitude(p, &(curr->key));
            if(comp < 0)
                curr = curr->left;
            else if(comp > 0)
                curr = curr->right;
            else
                break;
        }
        dim  = (dim + 1) % K;
    }
    
    // i.e. if the key did not exist
    if(curr == NULL)
    {
        t->n = t->n + 1;
        // create new node
        node* new = malloc(sizeof(node));
        if(new == NULL) return false;
        new->key = *p;
        new->left = NULL;
        new->right = NULL;

        // tree was empty
        if(t_is_empty){
            t->root = new;
        }
        else{
            // use parent's dimension to add
            if(dim == 0){
                if(p->lon < par->key.lon)
                    par->left = new;
                else if(p->lon > par->key.lon)
                    par->right = new;
            }
            if(dim == 1){
                if(p->lat < par->key.lat)
                    par->left = new;
                else if(p->lat > par->key.lat)
                    par->right = new;
            }
        }
        return true;
    }
    else // key already existed
        return false;
}


/**
 * Determines if the given tree contains a point with the same coordinates
 * as the given point.
 *
 * @param t a pointer to a valid k-d tree, non-NULL
 * @param p a pointer to a valid location, non-NULL
 * @return true if and only of the tree contains the location
 */
bool kdtree_contains(const kdtree *t, const location *p)
{
    if(t == NULL || p == NULL)
        return false;
    
    // Exactly the same as kdtree_add, just excluding par
    size_t dim = 0;
    node* curr = t->root; // root

    while(curr != NULL)
    {
        if(dim == 0) // compare lat
        {
            int comp = location_compare_latitude(p, &(curr->key));
            if(comp < 0)
                curr = curr->left;
            else if(comp > 0)
                curr = curr->right;
            else break;
        }
        if(dim == 1) // compare lon
        {
            int comp = location_compare_longitude(p, &(curr->key));
            if(comp < 0)
                curr = curr->left;
            else if(comp > 0)
                curr = curr->right;
            else break;
        }
        dim  = (dim + 1) % K;
    }

    // if curr was NULL, we reached the end of the tree without finding
    if(curr == NULL)
        return false;
    // else we broke out of wihile() at a non-NULL node with the same location
    else return true;
}


/**
 * Removes the point with the coordinates as the given point
 * from this k-d tree.  The tree need not be balanced
 * after the removal.  There is no effect if the point is not in the tree.
 *
 * @param t a pointer to a valid k-d tree, non-NULL
 * @param p a pointer to a valid location, non-NULL
 */
void kdtree_remove(kdtree *t, const location *p)
{
    if(t == NULL || p == NULL) return;

    int depth = 0;
    node* par = NULL;
    node* curr = t->root;

    // remember if we removed a point, so we can update the tree's counter
    bool* removed = malloc(sizeof(bool));
    *removed = false;

    t->root = internal_remove(par, curr, p, depth, removed);
    if(*removed == true) t->n = t->n - 1;
    free(removed);
    
    return;
}

node* internal_remove(node* par, node* curr, const location* p, int depth, bool* removed)
{
    // if the location is not present in the tree
    if(curr == NULL) return NULL;

    int dim = depth % K;
    int comp;
    if(dim == 0)
        comp = location_compare_latitude(p, &(curr->key));
    else if(dim == 1)
        comp = location_compare_longitude(p, &(curr->key));
    
    if(comp == 0) // found point
    {
        *removed = true;
        if(curr->left == NULL && curr-> right == NULL)
        {
            if(par == NULL); // this node was the root of the entire tree
            else if(par->left == curr) par->left = NULL;
            else if(par->right == curr) par->right = NULL;
            free(curr);
            curr = NULL;
        }
        // point has right subtree
        else if(curr->right != NULL)
        {
            node* min = find_min(curr->right, dim, depth);
            curr->key = min->key;
            curr->right = internal_remove(curr, curr->right, &(curr->key), depth+1, removed);
        }
        // point has no right subtree but has left subtree
        else if(curr->left != NULL)
        {
            node* max = find_max(curr->left, dim, depth);
            curr->key = max->key;
            curr->left = internal_remove(curr, curr->left, &(curr->key), depth+1, removed);
        }
    }
    else // look deeper
    {
        if(comp < 0)
            curr->left = internal_remove(curr, curr->left, p, depth+1, removed);
        if(comp > 0)
            curr->right = internal_remove(curr, curr->right, p, depth+1, removed);
    }
    
    return curr;
}


/**
 * Returns a dynamically allocated array containing the points in the
 * given tree in or on the borders of the (spherical) rectangle
 * defined by the given corners and sets the integer given as a
 * reference parameter to its size.  The points may be stored in the
 * array in an arbitrary order.  If there are no points in the
 * region, then the returned array may be empty, or it may be NULL.
 * It is the caller's responsibility ensure that the returned array
 * is eventually freed if it is not NULL.
 *
 * @param t a pointer to a valid k-d tree, non-NULL
 * @param sw a pointer to a valid location, non-NULL
 * @param ne a pointer to a valid location with latitude and longitude
 * both strictly greater than those in sw, non-NULL
 * @param n a pointer to an integer, non-NULL
 * @return a pointer to an array containing the points in the range, or NULL
 */
location *kdtree_range(const kdtree* t, const location *sw, const location *ne, int *n)
{
    if(t == NULL || sw == NULL || ne == NULL || n == NULL) return NULL;

    Array* x = malloc(sizeof(Array));
    x->count = 0;
    x->array = malloc((t->n) * sizeof(location));
    kdtree_range_for_each(t, sw, ne, add_to_arr, x);

    *n = x->count;
    location* result = x->array;
    free(x);

    return result;
}


/**
 * Passes the points in the given tree that are in or on the borders of the
 * (spherical) rectangle defined by the given corners to the given function
 * in an arbitrary order.  The last argument to this function is also passed
 * to the given function along with each point.
 *
 * @param t a pointer to a valid k-d tree, non-NULL
 * @param sw a pointer to a valid location, non-NULL
 * @param ne a pointer to a valid location with latitude and longitude
 * both strictly greater than those in sw, non-NULL
 * @param f a pointer to a function that takes a location and
 * the extra argument arg, non-NULL
 * @param arg a pointer to be passed as the extra argument to f
 */
void kdtree_range_for_each(const kdtree* t, const location *sw, const location *ne, void (*f)(const location *, void *), void *arg)
{
    if(t == NULL || sw == NULL || ne == NULL || f == NULL) return;
    
    double e = ne->lat;
    double w = sw->lat;
    double n = ne->lon;
    double s = sw->lon;
    int depth = 0;
    internal_range_for_each(t->root, depth, e, w, n, s, f, arg);
    
    return;
}

void internal_range_for_each(node* root, int depth, double e, double w, double n, double s, void (*f)(const location *, void *), void *arg)
{
    if(root == NULL) return;

    location* this = &(root->key);
    if(w<=this->lat && this->lat<=e && s<=this->lon && this->lon<=n)
        f(this, arg);
    int dim = depth % K;
    if(dim == 0)
    {
        if(w <= this->lat && this->lat <= e)
        {
            internal_range_for_each(root->left, depth+1, e, w, n, s, f, arg);
            internal_range_for_each(root->right, depth+1, e, w, n, s, f, arg);
        }
        else if(this->lat < w)
        {
            internal_range_for_each(root->right, depth+1, e, w, n, s, f, arg);
        }
        else if(e < this->lat)
        {
            internal_range_for_each(root->left, depth+1, e, w, n, s, f, arg);
        }
    }
    if(dim == 1)
    {
        if(s<=this->lon && this->lon<=n)
        {
            internal_range_for_each(root->left, depth+1, e, w, n, s, f, arg);
            internal_range_for_each(root->right, depth+1, e, w, n, s, f, arg);
        }
        else if(this->lon < s)
        {
            internal_range_for_each(root->right, depth+1, e, w, n, s, f, arg);
        }
        else if(n < this->lon)
        {
            internal_range_for_each(root->left, depth+1, e, w, n, s, f, arg);
        }
    }
}


/**
 * Destroys the given k-d tree.  The tree is invalid after being destroyed.
 *
 * @param t a pointer to a valid k-d tree, non-NULL
 */
void kdtree_destroy(kdtree *t)
{
    // error check
    if(t == NULL) return;

    // free nodes of the tree
    internal_destroy(t->root);

    free(t);
    return;
}

void internal_destroy(node* curr)
{
    if(curr == NULL) return;

    internal_destroy(curr->left);
    internal_destroy(curr->right);
    free(curr);
}

// ==========================================================================
// Auxiliary Functions
// ==========================================================================
/*
// This code is from: https://www.geeksforgeeks.org/print-binary-tree-2-dimensions/
#define COUNT 15

// Function to print binary tree in 2D
// It does reverse inorder traversal
void print2DUtil(node* root, int space)
{
    // Base case
    if (root == NULL)
        return;
 
    // Increase distance between levels
    space += COUNT;
 
    // Process right child first
    print2DUtil(root->right, space);
 
    // Print current node after space
    // count
    printf("\n");
    for (int i = COUNT; i < space; i++)
        printf(" ");
    printf("%.3lf, %.3lf\n", root->key.lat, root->key.lon);
 
    // Process left child
    print2DUtil(root->left, space);
}
 
// Wrapper over print2DUtil()
void print2D(kdtree* t)
{
    // Pass initial space count as 0
    print2DUtil(t->root, 0);
}
*/