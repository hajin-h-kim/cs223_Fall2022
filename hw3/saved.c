/* This assignment is for CS223 pset3 
This is the implementation of the track ADT */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "location.h"
#include "trackpoint.h"
#include "track.h"

typedef struct {
    trackpoint** trkptArr;
    int cap;
    int count;
} segment;

struct track
{
    // pointer to an array of segment pointers
    segment** segArr;
    int nSeg; // starts at 1
    int capSeg; // size of segArr, nTrkptArr, and lengthArr
    int* nTrkptArr;
    int totalTrkpt;
    double* lengthArr;
};

int comparator(const void* a, const void* b)
{
    // printf("inside the comparator\n");
    location l = (**(location**)a);
    location r = (**(location**)b);
    // printf("comparing longitudes %lf and %lf\n", l.lon, r.lon);
    
    // typecasting from double to int will occasionally round to unwanted values e.g. (int)0.1 = 0
    if(l.lon > r.lon) return -1;
    else if(l.lon < r.lon) return 1;
    else return 0;
    //return (location*)a->lon - (location*)b->lon; // check order of operation============
}

track* track_create()
{
    track* trk = malloc(sizeof(track));
    
    // initialize non-array variables
    trk->nSeg = 0;
    trk->capSeg = 10;
    trk->totalTrkpt = 0;

    // malloc & initialize array variables
    trk->segArr = calloc(trk->capSeg, sizeof(segment*));
    trk->nTrkptArr = calloc(trk->capSeg, sizeof(int));
    trk->lengthArr = calloc(trk->capSeg, sizeof(double));

    track_start_segment(trk);

    return trk;
}

void track_destroy(track *trk)
{
    for(int i = 0; i < trk->nSeg; ++i) // for all segments
    {
        for(int j = 0; j < trk->segArr[i]->count; ++j)
            trackpoint_destroy(trk->segArr[i]->trkptArr[j]);
        free(trk->segArr[i]->trkptArr);
        free(trk->segArr[i]);
    }
    // there are three (nSeg-long) array pointers to free
    free(trk->segArr);
    free(trk->nTrkptArr);
    free(trk->lengthArr);
    // finally, free the struct pointer itself
    free(trk);
}

int track_count_segments(const track *trk)
{
    return trk->nSeg;
}

int track_count_points(const track *trk, int i)
{
    return trk->nTrkptArr[i];
}

trackpoint *track_get_point(const track *trk, int i, int j)
{
    // return the point in the ith seg, jth point where both i, j start at 0
    return trackpoint_copy(trk->segArr[i]->trkptArr[j]);
}

double *track_get_lengths(const track *trk)
{
    double* ans = malloc(trk->nSeg * sizeof(double));
    memcpy(ans, trk->lengthArr, trk->nSeg);
    return ans;
}

void track_add_point(track *trk, const trackpoint *pt)
{
    trackpoint* copyOfTrkpt = trackpoint_copy(pt);

    // include pt to the last slot in the last segment
    // make things easier using an alias
    segment* thisSeg = trk->segArr[trk->nSeg-1];

    // check if the timestamp is strictly after the timestamp on last point
    if(thisSeg->count > 0) {
        if(trackpoint_time(thisSeg->trkptArr[thisSeg->count-1]) >= trackpoint_time(pt))
            return ;
    }
    else if (thisSeg->count == 0 && trk->nSeg > 1) {
        segment* lastSeg = trk->segArr[(trk->nSeg)-2];
        if(trackpoint_time(lastSeg->trkptArr[(lastSeg->count)-1]) >= trackpoint_time(pt))
            return ;
    }

    // step 0. update count in that segment struct
    (thisSeg->count)++;

    // step 1. check if we need to increase the length of trkptArr
    if (thisSeg->cap < thisSeg->count)
    {
        // double the cap
        thisSeg->cap *= 2;
        thisSeg->trkptArr = realloc(thisSeg->trkptArr, thisSeg->cap * sizeof(trackpoint*));
    }

    // step 2. store pt
    thisSeg->trkptArr[(thisSeg->count)-1] = copyOfTrkpt;

    // step 3-1. update other counting variables in trk
    trk->nTrkptArr[trk->nSeg-1]++;
    trk->totalTrkpt++;

    // step 3-2. update length array in trk
    location temp1;
    location temp2;
    if(thisSeg->count >= 2)
    {
        // trackpoint_location returns a location struct
        temp1 = trackpoint_location(thisSeg->trkptArr[thisSeg->count-2]);
        temp2 = trackpoint_location(thisSeg->trkptArr[thisSeg->count-1]);
        trk->lengthArr[trk->nSeg-1] += location_distance(&temp1, &temp2);
    }
    else ; // if there is only one trackpoint in the segment, length is still 0
    
    return ;
}

void track_start_segment(track *trk)
{
    // update trk counting variable
    trk->nSeg++;

    if(trk->nSeg > trk->capSeg) // need to resize
    {
        trk->capSeg *= 2;

        // realloc segArr
        trk->segArr = realloc(trk->segArr, trk->capSeg * sizeof(segment*) );
        trk->nTrkptArr = realloc(trk->nTrkptArr, trk->capSeg * sizeof(int));
        trk->lengthArr = realloc(trk->lengthArr, trk->capSeg * sizeof(double));
    }

    //segment* thisSeg = trk->segArr[trk->nSeg-1];
    trk->segArr[trk->nSeg-1] = malloc(sizeof(segment));
    
    segment* thisSeg = trk->segArr[trk->nSeg-1];
    thisSeg->cap = 10;
    thisSeg->count = 0;
    thisSeg->trkptArr = calloc(thisSeg->cap, sizeof(trackpoint*));

    return ;
}

void track_merge_segments(track *trk, int start, int end)
{
    // create an alias to the start (and final after merging) segment
    segment* finalSeg = trk->segArr[start];
    // create an alias we will use through the for loop
    segment* thisSeg;

    // start by making room in the start (and final after merging) segment's trkptArr
    trackpoint** tempTrkptArr;
    finalSeg->cap = trk->totalTrkpt; // choose an arbitrarily big cap
    tempTrkptArr = calloc(finalSeg->cap, sizeof(trackpoint*));
    for(int i = 0; i < finalSeg->count; ++i)
    {
        tempTrkptArr[i] = finalSeg->trkptArr[i];
    }
    free(finalSeg->trkptArr);
    finalSeg->trkptArr = tempTrkptArr;

    int nMergedSeg = end - start; // would have to loop nMergedSeg-1 times later
    int diff = end - start - 1; // caution: nMergedSeg =/= diff

    int* tempNTrkptArr = calloc(trk->capSeg, sizeof(int));
    double* tempLengthArr = calloc(trk->capSeg, sizeof(double));
    segment** tempSegArr = calloc(trk->capSeg, sizeof(segment*));
    
    // copy segment values before merging point
    // copy segArr[start] (i.e. finalSeg) too, so we can access/update it from anywhere
    for(int i = 0; i <= start; ++i)
    {
        tempNTrkptArr[i] = trk->nTrkptArr[i];
        tempLengthArr[i] = trk->lengthArr[i];
        tempSegArr[i] = trk->segArr[i];
    }

    // will use to update length of segments
    location temp1;
    location temp2;
    for(int i = start+1; i < end; ++i)
    {
        thisSeg = trk->segArr[i];

        // update the track's counting arrays
        tempNTrkptArr[start] +=  trk->nTrkptArr[i];
        tempLengthArr[start] +=  trk->lengthArr[i];
        // for location, we need to connect the last trackpoint of the final seg 
        // with the first trackpoint of the added segment
        temp1 = trackpoint_location(finalSeg->trkptArr[(finalSeg->count)-1]);
        temp2 = trackpoint_location(thisSeg->trkptArr[0]);
        tempLengthArr[start] += location_distance(&temp1, &temp2);

        // update the track's segment struct
        for(int j = 0; j < thisSeg->cap; ++j)
        {
            if(j < thisSeg->count)
            {
                finalSeg->trkptArr[finalSeg->count] = thisSeg->trkptArr[j];
                finalSeg->count++;
            }
        }
        // free thisSeg's pointer and trkptArr
        free(trk->segArr[i]->trkptArr); // free the old segment's trkptArr pointer
        free(trk->segArr[i]); // free the old segment struct =======================
        // QUESTION: is this different from free(thisSeg) ??========================
    }

    for(int i = end; i < trk->capSeg; ++i)
    {
        tempNTrkptArr[i - diff] = trk->nTrkptArr[i]; // end+1-diff == start+1
        tempLengthArr[i - diff] = trk->lengthArr[i];
        tempSegArr[i - diff] = trk->segArr[i];
    }
    // finally, replace nTrkpt, lengthArr, and segArr
    free(trk->nTrkptArr);
    trk->nTrkptArr = tempNTrkptArr;
    free(trk->lengthArr);
    trk->lengthArr = tempLengthArr;
    free(trk->segArr);
    trk->segArr = tempSegArr;

    // update remaining track counting variables
    trk->nSeg -= (nMergedSeg - 1);
}

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
 * 180 (exclusive)) longitude of the western edge.  The
 * distance (in degrees) between the bounds of adjacent
 * rows and columns is given by the last two
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
		    int ***map, int *rows, int *cols)
{
    // copy all trackpoints locations into a single array
    // while doing so, get lat: min/max 
    int nloc = trk->totalTrkpt;
    location** locArr = malloc(nloc * sizeof(location*));

    int k = 0;
    double south = INFINITY;
    double north = -INFINITY;
    for(int i = 0; i < trk->nSeg; ++i)
    {
        for(int j = 0; j < trk->segArr[i]->count; ++j)
        {
            locArr[k] = malloc(sizeof(location));
            *locArr[k] = trackpoint_location(trk->segArr[i]->trkptArr[j]);
            printf("trackpoint #%d: lat %lf lon %lf\n", k, locArr[k]->lat, locArr[k]->lon);
            if(south > locArr[k]->lat) south = locArr[k]->lat;
            if(north < locArr[k]->lat) north = locArr[k]->lat;
            k++;
        }
    }
    printf("finished making location array\n");
    // qsort to by increasing longitude (from -180 to 180)
    qsort(locArr, nloc, sizeof(location*), comparator);

    // find the smallest wedge
    // set comparing buffers to start with the eastmost trackpoint,
    // since that won't be considered in the for loop
    double west = locArr[0]->lon;
    double east = locArr[nloc - 1]->lon;
    double minWedge; // check later=======
    if(west < east)
        minWedge =  360 - (east - west);
    else
        minWedge = 360 - (east + west);
    double tempWedge;
    double left; // i.e. tempRight
    double right; // i.e. tempWest
    for(int i = 0; i < nloc-1; i ++)
    {
        left = locArr[i]->lon;
        right = locArr[i+1]->lon;
        if(left < right)
            tempWedge = 360 - (right - left);
        else
            tempWedge = 360 - (right + left);
        if(minWedge > tempWedge)
        {
            minWedge = tempWedge;
            west = right; // check
            east = left;
        }
        else if(minWedge == tempWedge)
        {
            // if there are multiple min wedges, find the one with the lowest normalized
            if(right < west)
            {
                west = right; // check
                east = left;
            }
        }
    }
    printf("north: %lf\n", north);
    printf("south: %lf\n", south);
    printf("east: %lf\n", east);
    printf("west: %lf\n", west);

    // check if we cross the prime meridian
    if(west < east)
    {
        // now get the number of rows and cols & update arguments through pointers
        *rows = ceil((north - south)/cell_height);
        *cols = ceil((east - west)/cell_width);
        printf("number of rows is %d, number of cols is %d\n", *rows, *cols);

        int** tempmap;
        int nrows = *rows;
        int ncols = *cols;
        tempmap = calloc((nrows), sizeof(int*));
        for(int i = 0; i < nrows; ++i) tempmap[i] = calloc((ncols), sizeof(int));

        printf("the initial map:\n");
        for(int i = 0; i < nrows; ++i)
        {
            for(int j = 0; j < ncols; ++j)
                printf("%d\t", tempmap[i][j]);
            printf("\n");
        }
        // now fill the map
        int ilat, ilon;
        for(int i = 0; i < nloc; ++i)
        {
            ilat = floor((-locArr[i]->lat + north)/cell_height);
            ilon = floor((locArr[i]->lon - west)/cell_width);
            printf("for point %d, lat index: %d, lon index: %d\n", i, ilat, ilon);
            tempmap[ilat][ilon]++;
            // free(locArr[i]);
        }
        // free(locArr);

        *map = tempmap;
    }
    else
    {
        // now get the number of rows and cols & update arguments through pointers
        *rows = ceil((north - south)/cell_height);
        *cols = ceil((east - west + 360)/cell_width);

        int** tempmap;
        int nrows = *rows;
        int ncols = *cols;
        tempmap = calloc((nrows), sizeof(int*));
        for(int i = 0; i < nrows; ++i) tempmap[i] = calloc((ncols), sizeof(int));
        map = &tempmap;

        // now fill the map
        int ilat, ilon;
        for(int i = 0; i < nloc; ++i)
        {
            ilat = floor((locArr[i]->lat - north)/cell_height);
            ilon = floor((locArr[i]->lon - west + 360)/cell_width);
            tempmap[ilat][ilon]++;
            // free(locArr[i]);
        }
        // free(locArr);

        *map = tempmap;
    }

    for(int i = 0; i < nloc; ++i) free(locArr[i]);
    free(locArr);
    
    return ;
}


/**
 * Determines: 1) the latitude of the northernmost and southernmost track points in the given track; and 2)
 * the meridian of longitude at the western edge of the smallest spherical wedge bounded by two meridians
 * that contains all the points in the track (the "western edge" for a nontrivial wedge being the one that,
 * when you move east from it along the equator, you stay in the wedge).  When there are multiple such wedges,
 * this function finds the one with the lowest normalized (adjusted to the range -180 (inclusive)
 * to 180 (exclusive)) longitude.
 *
 * @param trk a pointer to a valid, non-empty track
 * @param west a pointer to a double in which to record the western edge of the containing wedge
 * @param east a pointer to a double in which to record the eastern edge of the containing wedge
 * @param north a pointer to a double in which to record the latitude of the northernmost point
 * @param south a pointer to a double in which to record the latitude of the southernmost point
*/
static void track_bounds(const track *trk, double *west, double *east, double *north, double *south);
