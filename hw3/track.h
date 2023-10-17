#ifndef __TRACK_H__
#define __TRACK_H__

#include <stdbool.h>

#include "trackpoint.h"

/**
 * A track is a sequence of segments s_0, ..., s_{n-1}, where each segment
 * s_i is a sequence of track points t_i,0, ..., t_i,{n_i-1}, with each track
 * point recording the latitude, longitude, and time of some object
 * as it travels along its path.  Track points within a single segment
 * are recorded in order of strictly increasing time (so the time of
 * t_i_j will be strictly less than the time of t_i_{j+1} for two adjacent
 * track points in any segment), and segments are ordered so that the time
 * of the last track point in one segment is strictly less than the time
 * of the first track point in the next segment (so the time of t_i_{n_i-1}
 * i strictly less than the time of t_{i+1}_0).  There is always at
 * one segment, and only the last segment can be empty.
 *
 * Tracks have operations to add track points to the end of the current
 * (last) segment, start a new segment, merge segments by concatenating
 * a range of segments together, and query individual segments and points.
 */

typedef struct track track;

/**
 * Creates a track with one empty segment.
 *
 * @return the new track
 */
track *track_create();

/**
 * Destroys the given track, releasing all memory held by it and
 * rendering it invalid.
 *
 * @param trk a pointer to a valid track
 */
void track_destroy(track *trk);

/**
 * Returns the number of segments in the given track.
 *
 * @param trk a pointer to a valid track
 */
int track_count_segments(const track *trk);

/**
 * Returns the number of track points in the given segment of the given
 * track.
 *
 * @param trk a pointer to a valid track
 * @param i a nonnegative integer less than the number of segments in trk
 * @return the number of track points in the corresponding segment
 */
int track_count_points(const track *trk, int i);

/**
 * Returns a copy of the given point in this track.  The caller
 * takes ownership of the returned track point, and so is responsible
 * for ensuring its eventual destruction.
 *
 * @param trk a pointer to a valid track
 * @param i a nonnegative integer less than the number of segments in trk
 * @param j a nonnegative integer less than the number of points in segment i
 * of track trk
 */
trackpoint *track_get_point(const track *trk, int i, int j);

/**
 * Returns an array containing the length of each segment in this track.
 * The length of a segment is the sum of the distances between each point
 * in it and the next point in the segment.
 * The length of a segment with fewer than two
 * points is zero.  The caller takes ownership of the returned array
 * and so is responsible for ensuring that it is eventually freed.
 *
 * @param trk a pointer to a valid track
 */
double *track_get_lengths(const track *trk);

/**
 * Adds a copy of the given point to the last segment in this track.
 * The point is not added and there is no change to the track if there
 * is a last point in the track (the last point in the current segment
 * or the last point on the previous segment if the current segment is
 * empty) and the timestamp on the new point is not strictly after the
 * timestamp on the last point.  This function must execute in
 * amortized O(1) time (so a sequence of n consecutive operations must
 * work in worst-case O(n) time).
 *
 * @param trk a pointer to a valid track
 * @param pt a pointer to a track point with a timestamp strictly after
 * the timestamp of last track point in the last non-empty segment in this
 * track (if there is such a point).
 */
void track_add_point(track *trk, const trackpoint *pt);

/**
 * Starts a new empty segment in the given track and makes that the current
 * (last) segment.
 *
 * @param trk a pointer to a valid track whose current (last)
 * segment is non-empty
 */
void track_start_segment(track *trk);

/**
 * Merges the given range of segments in this track into one.  The
 * range includes the starting index and excludes the ending index.
 * The resulting segment replaces the first merged one and later segments
 * are moved up to replace the other merged segments.
 *
 * @param trk a pointer to a valid track
 * @param start an integer greater than or equal to 0 and strictly less than
 * the number if segments in trk
 * @param end an integer greater than or equal to start and less than or
 * equal to the number of segments in trk
 */
void track_merge_segments(track *trk, int start, int end);

/**
 * Creates a heatmap of the given track.  The heatmap will be a
 * rectangular 2-D array with each row separately allocated.  The last
 * three parameters are (simulated) reference parameters used to return
 * the heatmap and its dimensions.  Each element in the heatmap
 * represents an area bounded by two circles of latitude and two
 * meridians of longitude.  The circle of latitude bounding the top of
 * the top row is the northernmost (highest) latitude of any
 * track point in the given track.  The meridian bounding the left of
 * the first column is the western edge of the smallest spherical
 * wedge bounded by two meridians that contains all the points in the
 * track (the "western edge" for a nontrivial wedge being the one
 * that, when you move east from it along the equator, you stay in the
 * wedge).  When there are multiple such wedges, choose the one with
 * the lowest normalized (adjusted to the range -180 (inclusive) to
 * 180 (exclusive)) longitude.  The distance (in degrees) between the
 * bounds of adjacent rows and columns is given by the last two
 * parameters.  The heat map will have just enough rows and just
 * enough columns so that all points in the track fall into some cell.
 * The value in each entry in the heatmap is the number of track points
 * located in the corresponding cell.  If a track point is on the
 * border of two or more cells then it is counted in the bottommost
 * and rightmost cell it is on the border of, except the southernmost row
 * and easternmost column include their southern and eastern border
 * respectively, and neither will contain only points on their
 * northern or western border.  If the eastermost cells
 * have wrapped around to overlap the westernmost cells then the
 * points that belong to both are placed in the westernmost cells.
 * If there are no track points in the track then the function
 * creates a 1x1 heatmap with the single element having a value of 0.
 * The caller takes ownership of the returned array (both the
 * array of rows and the arrays for each individual row; the caller
 * must ensure that those arrays are eventually freed).
 *
 * @param trk a pointer to a valid track
 * @param cell_width a positive double less than or equal to 360.0
 * @param cell_height a positive double less than or equal to 180.0
 * @param map a pointer to a pointer to a 2-D array of ints
 * @param rows a pointer to an int, non-NULL
 * @param cols a pointer to an int, non-NULL
 */
void track_heatmap(const track *trk, double cell_width, double cell_height,
		    int ***map, int *rows, int *cols);

#endif

