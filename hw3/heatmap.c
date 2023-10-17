#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "track.h"
#include "trackpoint.h"
#include "location.h"

#define INITIAL_LINE_LENGTH (2)

char *getLine(void);

int main(int argc, char **argv)
{
    if(argc != 5) return argc;

    double cell_width;
    // atof returns 0 when it fails to convert to double
    if( (cell_width = atof(argv[1])) == 0) return cell_width;
    double cell_height;
    if( (cell_height = atof(argv[2])) == 0 ) return cell_height;
    char* art = argv[3];
    int n = atoi(argv[4]);
    // if n is not a positive integer
    if(!n) return n;
    
    double tempLat, tempLon;
    long tempT;
    trackpoint* tempTrkpt;
    track* trk = track_create();

    // read in from input file
    char* line = getLine();
    int emptyTrk = 0;
    if(strcmp(line, "ENTER") == 0)
    {
        emptyTrk = 1;
    }
    
    // read input with scanf using a while loop
    while( !emptyTrk && (sscanf(line, "%lf %lf %ld", &tempLat, &tempLon, &tempT) == 3))
    {
        tempTrkpt = trackpoint_create(tempLat, tempLon, tempT);
        track_add_point(trk, tempTrkpt);
        trackpoint_destroy(tempTrkpt);

        free(line);
        line = getLine();
        if(strcmp(line, "ENTER") == 0) line = getLine();
    }
    free(line);

    int*** map = calloc(1, sizeof(int**));
    int* rows = calloc(1, sizeof(int));
    int* cols = calloc(1, sizeof(int));
    track_heatmap(trk, cell_width, cell_height, map, rows, cols);

    int index;
    for(int i = 0; i < *rows; ++i)
    {
        for(int j = 0; j < *cols; ++j)
        {
            // typecast to int
            // no need for floor(), because results are all positive doubles
            index = (int)((*map)[i][j]/n);
            if(index >= strlen(art)) index = strlen(art) - 1;
            printf("%c", art[index]);
        }
        printf("\n");
    }

    // free track
    track_destroy(trk);
    // free heatmap
    for(int i = 0; i < *rows; ++i) free((*map)[i]);
    free(*map);
    free(map);
    free(rows);
    free(cols);

    return 0;
}

// This code was provided by Professor Aspnes's notes
// http://cs.yale.edu/homes/aspnes/classes/223/notes.html#Reading_and_writing_floating-point_numbers
char *getLine(void)
{
    char *line;
    int size = INITIAL_LINE_LENGTH;   /* how much space do I have in line? */
    int length; /* how many characters have I used */
    int c;

    line = malloc(size);
    assert(line);

    length = 0;

    while((c = getchar()) != EOF && c != '\n') {
        if(length >= size-1) {
            /* need more space! */
            size *= 2;

            /* make length equal to new size */
            /* copy contents if necessary */
            line = realloc(line, size);
        }

        line[length++] = c;
    }

    line[length] = '\0';

    // newly added to account for empty lines between segments
    if(line[0] == '\0' &&  c == '\n')
    {
        free(line);
        return "ENTER";
    }
    return line;
}
