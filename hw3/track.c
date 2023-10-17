/* This assignment is for Fall 2022 CS223 pset3 
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
    location l = (**(location**)a);
    location r = (**(location**)b);
    
    // typecasting from double to int will occasionally round to unwanted values e.g. (int)0.1 = 0
    if(l.lon > r.lon) return 1;
    else if(l.lon < r.lon) return -1;
    else return 0;
}

//===========================================================================
// track_create function
//===========================================================================
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

//===========================================================================
// track_destroy function
//===========================================================================
void track_destroy(track *trk)
{
    for(int i = 0; i < trk->nSeg; ++i) // for all segments
    {
        for(int j = 0; j < trk->segArr[i]->count; ++j)
            // destroy copy of the trackpoints we made
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

//===========================================================================
// track_count_segments function
//===========================================================================
int track_count_segments(const track *trk)
{
    return trk->nSeg;
}

//===========================================================================
// track_count_points function
//===========================================================================
int track_count_points(const track *trk, int i)
{
    return trk->nTrkptArr[i];
}

//===========================================================================
// track_get_point function
//===========================================================================
trackpoint *track_get_point(const track *trk, int i, int j)
{
    // return the point in the ith seg, jth point where both i, j start at 0
    return trackpoint_copy(trk->segArr[i]->trkptArr[j]);
}

//===========================================================================
// track_get_lengths function
//===========================================================================
double *track_get_lengths(const track *trk)
{
    double* ans = malloc(trk->nSeg * sizeof(double));
    memcpy(ans, trk->lengthArr, trk->nSeg);
    return ans;
}

//===========================================================================
// track_add_point function
//===========================================================================
void track_add_point(track *trk, const trackpoint *pt)
{
    // check if the location is valid
    // if location isn't valid, return;
    location ptloc = trackpoint_location(pt);
    if(!location_validate(&ptloc)) return;

    // if it is, make copy of trackpoint to add to the track
    trackpoint* copyOfTrkpt = trackpoint_copy(pt);

    // include pt to the last slot in the last segment
    // make things easier using an alias
    segment* thisSeg = trk->segArr[trk->nSeg-1];

    // check if the timestamp is strictly after the timestamp on last point
    if(thisSeg->count > 0) {
        if(trackpoint_time(thisSeg->trkptArr[thisSeg->count-1]) >= trackpoint_time(pt))
        {
            free(copyOfTrkpt);
            return;
        }
    }
    else if (thisSeg->count == 0 && trk->nSeg > 1) {
        segment* lastSeg = trk->segArr[(trk->nSeg)-2];
        if(trackpoint_time(lastSeg->trkptArr[(lastSeg->count)-1]) >= trackpoint_time(pt))
        {
            free(copyOfTrkpt);
            return;
        }
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

//===========================================================================
// track_start_segment function
//===========================================================================
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

//===========================================================================
// track_merge_segments function
//===========================================================================
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
        free(trk->segArr[i]); // free the old segment struct
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

//===========================================================================
// track_heatmap function
//===========================================================================
void track_heatmap(const track *trk, double cell_width, double cell_height,
		    int ***map, int *rows, int *cols)
{
    // copy all trackpoints locations into a single array
    // while doing so, get lat: min/max 
    int nloc = trk->totalTrkpt;
    if(nloc == 0)
    {
        *rows = 1;
        *cols = 1;
        int** tempmap;
        tempmap = calloc((*rows), sizeof(int*));
        tempmap[0] = calloc((*cols), sizeof(int));
        tempmap[0][0] = 0;

        *map = tempmap;
        return ;
    }

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
            if(south > locArr[k]->lat) south = locArr[k]->lat;
            if(north < locArr[k]->lat) north = locArr[k]->lat;
            k++;
        }
    }
    
    // qsort to by increasing longitude (from -180 to 180)
    qsort(locArr, nloc, sizeof(location*), comparator);
    
    // find the smallest wedge
    // set comparing buffers to start with the eastmost trackpoint,
    // since that won't be considered in the for loop
    double west = locArr[0]->lon;
    double east = locArr[nloc - 1]->lon;
    double minWedge = locArr[nloc - 1]->lon - locArr[0]->lon;
    double tempWedge;
    // try making i the westernmost point
    for(int i = 0; i < nloc-1; i ++)
    {
        tempWedge = 360 - (locArr[i+1]->lon - locArr[i]->lon);
        if(minWedge > tempWedge)
        {
            minWedge = tempWedge;
            west = locArr[i+1]->lon; // check
            east = locArr[i]->lon;
        }
        else if(minWedge == tempWedge)
        {
            // if there are multiple min wedges, find the one with the lowest normalized
            if(locArr[i+1]->lon < west)
            {
                west = locArr[i+1]->lon; // check
                east = locArr[i]->lon;
            }
        }
    }

    // now get the number of rows and cols & update arguments through pointers
    *rows = ceil((north - south)/cell_height);
    *cols = ceil(minWedge/cell_width);
    if(*rows == 0) (*rows)++;
    if(*cols == 0) (*cols)++;
    
    int nrows = *rows;
    int ncols = *cols;
    int** tempmap;
    tempmap = calloc((nrows), sizeof(int*));
    for(int i = 0; i < nrows; ++i) tempmap[i] = calloc((ncols), sizeof(int));

    // now fill the map
    int ilat, ilon;
    // check if we cross the prime meridian
    if(west <= east)
    {
        for(int i = 0; i < nloc; ++i)
        {
            ilat = floor((north - locArr[i]->lat)/cell_height);
                if(ilat == (double)nrows) ilat--;
            ilon = floor((locArr[i]->lon - west)/cell_width);
                if(ilon == (double)ncols) ilon--;
            
            tempmap[ilat][ilon]++;
        }
    }
    else
    {
        for(int i = 0; i < nloc; ++i)
        {
            ilat = floor((north - locArr[i]->lat)/cell_height);
            if(ilat == (double)nrows) ilat--;

            if(west <= locArr[i]->lon && locArr[i]->lon < 180)
                ilon = floor((locArr[i]->lon - west)/cell_width);
            else
                ilon = floor((locArr[i]->lon - west + 360)/cell_width);
            if(ilon == (double)ncols) ilon--;

            tempmap[ilat][ilon]++;
        }
    }

    *map = tempmap;

    for(int i = 0; i < nloc; ++i) free(locArr[i]);
    free(locArr);

    return ;
}