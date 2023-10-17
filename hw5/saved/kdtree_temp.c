#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "location.h"
#include "kdtree.h"
#include "kdtree_helpers.h"

typedef struct _node{
    location key;
    struct _node *left, *right;
} node;

struct _kdtree{
    node* root;
    int n;
};

typedef struct {
    int count;
    location* array;
} Array;

// ==========================================================================
// Helper Functions
// ==========================================================================
/*
 * compares two location structs
 */
// bool issame(const location* a, const location* b){
//     printf("Do I even enter this function?\n");
//     printf("the locations I am comparing are:\n");
//     printf("curr's key: %lf, %lf\tnew key: %lf, %lf\n", a->lat, a->lon, b->lat, b->lon);
//     return (a->lat == b->lat) && (a->lon == b->lon);
// }

void internal_destroy(node* curr);
node* internal_create(kdtree*t, location* cut, location* other, int n, int d);

#define K 2

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

bool in_cut_left(location this, location median, int d)
{
    if(d == 0)
        if(location_compare_latitude(&this, &median) < 0) return true;

    if(d == 1)
        if(location_compare_longitude(&this, &median) < 0) return true;

    return false;
}

bool in_cut_right(location this, location median, int d)
{
    if(d == 0)
        if(location_compare_latitude(&this, &median) > 0) return true;

    if(d == 1)
        if(location_compare_longitude(&this, &median) > 0) return true;
    
    return false;
}

node* internal_create(kdtree*t, location* cut, location* other, int n, int d){
    // base case
    if (n == 0) return NULL;
    
    int median = n/2;
    int n_left = median;
    int n_right = n-median-1;

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

    // fill other_left/right. parse through the other array and send each location to either left or right
    int ileft = 0;
    int iright = 0;
    for(int i = 0; i < n; ++i)
    {
        if(in_cut_left(cut[i], cut[median], d)) other_left[ileft++] = other[i];
        else if(in_cut_right(cut[i], cut[median], d)) other_right[iright++] = other[i];
    }

    d = (d + 1) % K;
    
    newnode->key = cut[median];
    newnode->left = internal_create(t, other_left, cut_left, n_left, d);
    newnode->right = internal_create(t, other_right, cut_right, n_right, d);

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
    // printf("===Adding location===\n");
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
            else{
                // printf("! location already existed\n");
                break;
            }
        }
        if(dim == 1) // compare lon
        {
            int comp = location_compare_longitude(p, &(curr->key));
            if(comp < 0)
                curr = curr->left;
            else if(comp > 0)
                curr = curr->right;
            else{
                // printf("! location already existed\n");
                break;
            }
        }
        dim  = (dim + 1) % K;
    }
    
    // i.e. if the key did not exist
    if(curr == NULL)
    {
        // create new node
        node* new = malloc(sizeof(node));
        if(new == NULL) return false;
        new->key = *p;
        new->left = NULL;
        new->right = NULL;

        // tree was empty
        if(t_is_empty){
            // printf("Tree was empty, ");
            t->root = new;
        }
        else{
            // printf("Tree was not empty: at dim %d, ", dim);
            // use parent's dimension to add
            if(dim == 0){
                if(p->lon < par->key.lon)
                {
                    // printf("to the left, ");
                    par->left = new;
                }
                else if(p->lon > par->key.lon)
                {
                    // printf("to the right, ");
                    par->right = new;
                }
            }
            if(dim == 1){
                if(p->lat < par->key.lat)
                {
                    // printf("to the left, ");
                    par->left = new;
                }
                else if(p->lat > par->key.lat)
                {
                    // printf("to the right, ");
                    par->right = new;
                }
            }
        }
        // printf("added location %lf, %lf.\n", new->key.lat, new->key.lon);
        return true;
    }
    else // key already existed
        return false;
}

bool internal_contains(node* curr, const location *p, int depth)
{
    if(curr == NULL) return false;

    int dim = depth % K;
    int comp;
    if(dim == 0) comp = location_compare_latitude(p, &(curr->key));
    if(dim == 1) comp = location_compare_longitude(p, &(curr->key));

    if(comp < 0) internal_contains(curr->left, p, depth+1);
    else if(comp > 0) internal_contains(curr->right, p, depth+1);
    else return true;
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

    int depth = 0;
    return internal_contains(t->root, p, depth);
}

node* min_of_three(node* x, node* y, node* z, int dim_cut)
{
    // we know fo sure x (root from find_min) is not NULL
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

node* max_of_three(node* x, node* y, node* z, int dim_cut)
{
    // we know fo sure x (root from find_min) is not NULL
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

node* find_max(node* root, int dim_cut, int depth)
{
    if(root == NULL) return NULL;

    int dim = depth % K;
    if(dim == dim_cut)
    {
        if(root->right != NULL)
            return find_max(root->left, dim_cut, depth+1);
        else
            return root;
    }
    else
    {
        return max_of_three(root, find_max(root->left, dim_cut, depth+1),
                                  find_max(root->right, dim_cut, depth+1), dim_cut);
    }
}

node* internal_remove(node* par, node* curr, const location* p, int depth)
{
    printf("2. in internal_remove\n");
    // location not present in tree
    if(curr == NULL) return NULL;

    int dim = depth % K;
    int comp;
    if(dim == 0)
        comp = location_compare_latitude(p, &(curr->key));
    else if(dim == 1)
        comp = location_compare_longitude(p, &(curr->key));
    
    if(comp == 0) // found point
    {
        if(curr->left == NULL && curr-> right == NULL)
        {
            if(par == NULL); // this node was the root of the entire tree
            else if(par->left == curr) par->left = NULL;
            else if(par->right == curr) par->right = NULL;
            free(curr);
            curr = NULL;
            printf("Freed the leaf node\n");
        }
        // point has right subtree
        else if(curr->right != NULL)
        {
            node* min = find_min(curr->right, dim, depth);
            curr->key = min->key;
            curr->right = internal_remove(curr, curr->right, &(curr->key), depth+1);
        }
        // point has no right subtree but has left subtree
        else if(curr->left != NULL)
        {
            node* max = find_max(curr->left, dim, depth);
            curr->key = max->key;
            curr->left = internal_remove(curr, curr->left, &(curr->key), depth+1);
        }
    }
    else // look deeper
    {
        if(comp < 0)
            curr->left = internal_remove(curr, curr->left, p, depth+1);
        if(comp > 0)
            curr->right = internal_remove(curr, curr->right, p, depth+1);
    }
    
    return curr;
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
    printf("1. in kdtree_remove\n");
    if(t == NULL || p == NULL) return;

    int depth = 0;
    node* par = NULL;
    node* curr = t->root;
    t->root = internal_remove(par, curr, p, depth);
}


void add_to_arr(const location* loc, void* a)
{
    // printf("4. inside add_to_arr function\n");
    // if(loc == NULL) printf("loc is NULL\n");
    // if(a == NULL) printf("Array struct a is NULL\n");

    Array* x = (Array*) a;
    // printf("Initial count is %d, ", x->count);
    x->count = x->count+1;
    // printf("updated count is %d\n", x->count);
    x->array = realloc(x->array, (x->count)*sizeof(location));
    x->array[x->count-1] = *loc;
    // if(x->count == 20)
    // for(int i = 0; i < x->count; ++i)
    // printf("%.2lf, %.2lf\n", x->array[i].lat, x->array[i].lon);
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

    Array* x = calloc(1, sizeof(Array));
    // x->count = 0;
    // x->array = NULL;

    // printf("1. inside kdtree_range\n");
    kdtree_range_for_each(t, sw, ne, add_to_arr, x);
    // printf("6. back in kdtree_range after finishing kdtree_for_each\n");

    *n = x->count;
    location* result = x->array;
    
    // if(result == NULL) printf("before freeing the struct, array is NULL\n");
    free(x);
    // if(result == NULL) printf("after freeing the struct, array is NULL\n");
    return result;
}

void internal_range_for_each(node* root, int depth, double e, double w, double n, double s, void (*f)(const location *, void *), void *arg)
{
    // printf("3. inside internal_range_for_each\n");
    if(root == NULL) return;

    // printf("e: %lf\n", e);
    // printf("w: %lf\n", w);
    // printf("n: %lf\n", n);
    // printf("s: %lf\n", s);
    location* this = &(root->key);
    // printf("this location: lat %lf, lon %lf\n", this->lat, this->lon);
    if(w<=this->lat && this->lat<=e && s<=this->lon && this->lon<=n)
        f(this, arg);
    // printf("5. successfully executed add_to_arr function\n");
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
    // printf("2. inside kdtree_range_for_each\n");
    double e = ne->lat;
    double w = sw->lat;
    double n = ne->lon;
    double s = sw->lon;
    int depth = 0;
    internal_range_for_each(t->root, depth, e, w, n, s, f, arg);
    
    return;
}

void internal_destroy(node* curr)
{
    if(curr == NULL) return;

    internal_destroy(curr->left);
    internal_destroy(curr->right);
    free(curr);
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
