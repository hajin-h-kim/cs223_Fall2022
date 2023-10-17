#ifndef __KDTREE_H__
#define __KDTREE_H__

#include <stdbool.h>
#include "location.h"

/**
 * A set of geographic locations given by latitude and longitude
 * implemented using a k-d tree, where k = 2.  For all functions,
 * points are considered different based on the values of the
 * coordinates that are passed in, even if the coordinates represent
 * the same physical point.  For example, longitude -180 is
 * considered different than longitude 180, and points at the same pole
 * but with different longitudes are considered different.
 */
typedef struct _kdtree kdtree;


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
kdtree *kdtree_create(const location *pts, int n);


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
bool kdtree_add(kdtree *t, const location *p);


/**
 * Determines if the given tree contains a point with the same coordinates
 * as the given point.
 *
 * @param t a pointer to a valid k-d tree, non-NULL
 * @param p a pointer to a valid location, non-NULL
 * @return true if and only of the tree contains the location
 */
bool kdtree_contains(const kdtree *t, const location *p);


/**
 * Removes the point with the coordinates as the given point
 * from this k-d tree.  The tree need not be balanced
 * after the removal.  There is no effect if the point is not in the tree.
 *
 * @param t a pointer to a valid k-d tree, non-NULL
 * @param p a pointer to a valid location, non-NULL
 */
void kdtree_remove(kdtree *t, const location *p);


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
location *kdtree_range(const kdtree* r, const location *sw, const location *ne, int *n);


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
void kdtree_range_for_each(const kdtree* r, const location *sw, const location *ne, void (*f)(const location *, void *), void *arg);


/**
 * Destroys the given k-d tree.  The tree is invalid after being destroyed.
 *
 * @param t a pointer to a valid k-d tree, non-NULL
 */
void kdtree_destroy(kdtree *t);

// void print2D(kdtree* t);

#endif
