/* This assignment is for CS223 pset2
 * TSP given a list of locations and the distances between every pair of them, 
 * determining the shortest tour that starts and ends at the same city 
 * and visits each other city exactly once. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include "location.h"

typedef enum { GIVEN, NEAREST, FARTHEST } heuristics;
typedef struct city {
    char* name;
    location loc;
    int index;
    bool added;
} City;

int comparator(const void* a, const void* b)
{
    char* string1 = (*((City**)a)) -> name;
    char* string2 = (*((City**)b)) -> name;
    return strcmp(string1, string2);
}

void insert(City** tour, City* new_city, int tourlength, int insert_index)
{
    for(int i = tourlength; i > insert_index; --i)
    {
        tour[i] = tour[i-1];
    }
    tour[insert_index] = new_city;
}

bool notInTour(City** tour, City* new_city, int tourlength)
{
    bool ans = false;
    for(int i = 0; i < tourlength; i++)
    {
        if(tour[i] == new_city) ans = true;
    }
    return ans;
}

int main(int argc, char **argv)
{
    char* filename;
    FILE* inputFile;

    //=========================================================================
    // READ IN COMMAND LINE
    //=========================================================================

    // Print out an error msg when there are no command-line arguments
    if(argc <= 1)
    {
        fprintf(stderr, "TSP: missing filename\n");
        return 1;
    }

    // Open the input file and make sure it opened properly.
    // Check: Print out an error msg when there was an error opening the file
    filename = argv[1];
    inputFile = fopen(filename, "r");
    if (inputFile == NULL)
    {
        fprintf(stderr, "TSP: could not open %s\n", filename);
        return 1;
    }

    // Look for cities
    int startOfCity  = 0;
    for(int i=2; i<argc; i++)
    {
        if(argv[i][0] != (int)'-')
        {
            if(strcmp(argv[i-1], "-insert") != 0)
            {
                // We found the first city name if the argument:
                // (1) does not start with a hyphen and (2) isn't a modifier of the previous heuristic
                startOfCity = i;
                break;
            }
        }
    }

    // Check: must have at least two cities
    int nCity = argc - startOfCity;
    if((nCity) < 2)
    {
        fprintf(stderr, "TSP: too few cities\n");
        fclose(inputFile);
        return 1;
    }

    //=========================================================================
    // STORE CITIES IN ASCII ORDER
    //=========================================================================
    City** initial_cities = malloc(sizeof(City*) * nCity); // pointer to an array of pointer to cities
    City** ordered_cities = malloc(sizeof(City*) * nCity); // pointer to the same array ASCII-betical order
    City** tour = malloc(sizeof(City*) * nCity);

    // Store city names in both the initial cities array & ordered cities array
    for(int i=startOfCity; i<argc; i++)
    {
        int j = i - startOfCity;
        initial_cities[j] = malloc(sizeof(City));
        initial_cities[j]->name = argv[i];
        initial_cities[j]->index = j;
        initial_cities[j]->added = false;
        ordered_cities[j] = initial_cities[j];
    }

    // Use qsort to order cities in ordered_cities
    qsort(ordered_cities, nCity, sizeof(City*), comparator);

    //=========================================================================
    // READ IN LOCATION FROM FILE
    //=========================================================================
    int iCity = 0;
    int iChar = 0; // create buffer to store potential cities
    char* buffer = malloc((strlen(ordered_cities[iCity] -> name) + 2) * sizeof(char));
    char c;
    while( iCity < nCity && ((c = fgetc(inputFile)) != EOF) ) // UPDATE
    {
        // UPDATE: make sure if these variables should be outside the while loop
        int strlength = strlen(ordered_cities[iCity] -> name);

        // Reached newline
        if(c == '\n')
        {
            iChar = 0;
            free(buffer);
            buffer = malloc((strlength + 2) * sizeof(char));
        }
        else if(iChar < (strlength+1))
        {
            buffer[iChar++] = c;
        }

        if(iChar == (strlength+1))
        {
            buffer[iChar++] = '\0';
        }

        if(iChar == (strlength+2))
        {

            char cityname[strlength+2];
            //char* cityname = malloc((strlength+2)*sizeof(char));
            strcpy(cityname, ordered_cities[iCity] -> name);
            strcat(cityname, ",\0");

            if(strcmp(buffer, cityname) == 0)
            {
                fscanf(inputFile, "%lf,%lf", &(*ordered_cities[iCity]).loc.lat
                               , &(*ordered_cities[iCity]).loc.lon);
                // check if location is not valid
                if(!location_validate( &(*ordered_cities[iCity]).loc ))
                {
                    fprintf(stderr, "Invalid location\n");
                    fclose(inputFile);
                    return 1;
                }
                iCity++;
                iChar = strlength*2; // dummy value
            }
        }
    }

    if(buffer != NULL) free(buffer);

    // Check: if one of the cities on the command line is not present in the file
    if(iCity < nCity)
    {
        fprintf(stderr, "TSP: could not find location of %s\n", ordered_cities[iCity] -> name);
        fclose(inputFile);
        return 1;
    }

    // Close the input file, since we're done reading from it.
    fclose(inputFile);

    //=========================================================================
    // READ IN HEURISTICS
    //=========================================================================
    heuristics command[argc-2];
    int iCommand = 0, nCommand = 0;

    for(int i = 2; i < startOfCity; i++) // start at 2 because the first two are not heuristics
    {
        if(strcmp(argv[i], "-given") == 0){
            // do nothing
            command[iCommand++] = GIVEN;
        } else if (strcmp(argv[i], "-insert") == 0){
            i++;
            if(strcmp(argv[i], "nearest") == 0){
                command[iCommand++] = NEAREST;
            } else if(strcmp(argv[i], "farthest")==0){
                command[iCommand++] = FARTHEST;
            } else {
                fprintf(stderr, "TSP: invalid heuristic arguments\n");
                fclose(inputFile);
                return 1;
            }
        } 
    }
    nCommand = iCommand;
    //=========================================================================
    // CALCULATE AND PRINT TOURS
    //=========================================================================

    double** distances = malloc(sizeof(double*) * nCity);
    for (int i = 0; i < nCity; ++i) distances[i] = malloc(sizeof(double) * nCity);

    double min_replaced;
    int n = 2;
    // go through command array
    for(iCommand = 0; iCommand < nCommand; iCommand++) // UPDATE: while loop or for loop?
    {
        double total = 0;
        switch(command[iCommand])
        {
            case GIVEN:
                printf("-given            :");

                for(iCity = 0; iCity < (nCity - 1); iCity++)
                {
                    total += location_distance( &(initial_cities[iCity]->loc), &(initial_cities[iCity+1]->loc) );
                }
                total += location_distance( &(initial_cities[nCity-1]->loc), &(initial_cities[0]->loc) );
                printf("%13.2lf", total); // UPDATE: decimals and scientific notation
                
                for(int i = 0; i < nCity; i++)
                {
                    printf(" %s", initial_cities[i] -> name);
                }
                printf(" %s\n", initial_cities[0] -> name);
                break;


            case NEAREST:
                printf("-insert nearest   :");
                // Now calculate and store the location between points
                double minDistToTour = location_distance( &(initial_cities[0]->loc), &(initial_cities[1]->loc) );
                
                // Find first two cities & Fill in distances array
                for(int i = 0; i < nCity; ++i) // UPDATE: nCity or nCity-1
                {
                    for(int j = 0; j < nCity; ++j)
                    {
                        // // UPDATE: initial_cities or ordered_cities?
                        distances[i][j] = location_distance( &(initial_cities[i]->loc), &(initial_cities[j]->loc) );

                        if((minDistToTour > distances[i][j]) && (i != j))
                        {
                            minDistToTour = distances[i][j];
                            tour[0] = initial_cities[i]; // store the address of a city
                            tour[1] = initial_cities[j];
                            // tour[1] points to the same address (of a city) as ordered_cities[j]
                        }
                    }
                }

                tour[0]->added = true;
                tour[1]->added = true;
                total = distances[tour[0]->index][tour[1]->index];

                int closest_city;
                int insert_index;
                double tempTotal;

                //Order the remaining cities
                for(n = 2; n < nCity; ++n) // for all cities to add into the tour
                {
                    int temp = nCity-n;
                    printf("When there are %d cities in the tour and %d remaining: \n", n, temp);

                    // Find the closest city and its distance
                    minDistToTour = INFINITY;
                    for(int i = 0; i < nCity; ++i) // all the cities (out the tour)
                    {
                        if (!(initial_cities[i]->added)) // if the city isn't in tour
                        {
                            printf("\t-first for loop or nontour cities: consider city[%d] %s\n", i, initial_cities[i]->name);

                            for(int j = 0; j < n; ++j) // check which city in the tour its closest to
                            {
                                printf("\t\t-second for loop: connect with city[%d] %s in the tour: ", j, tour[j]->name);
                                
                                if((minDistToTour > distances[i][tour[j]->index])) //&& notInTour(tour, initial_cities[i], n)
                                {
                                    minDistToTour = distances[i][tour[j]->index];
                                    closest_city = i;
                                    printf("updated minDisToTour is %lf", minDistToTour);
                                }
                                
                                printf("\n");
                            }
                        }
                    }

                    printf("Trying to insert city %s\n", initial_cities[closest_city]->name);
                    initial_cities[closest_city]->added = true;

                    // Find place to insert it in
                    min_replaced = total - distances[tour[0]->index][tour[n-1]->index]
                                        + distances[closest_city][tour[0]->index]
                                        + distances[closest_city][tour[n-1]->index];
                                        
                    insert_index = 0;
                    for(int i = 1; i < n; ++i)
                    {
                        tempTotal = total - distances[tour[i-1]->index][tour[i]->index]
                                    + distances[closest_city][tour[i-1]->index]
                                    + distances[closest_city][tour[i]->index];

                        if(min_replaced > tempTotal)
                        {
                            min_replaced = tempTotal;
                            insert_index = i;
                        }
                    }
                    insert(tour, initial_cities[closest_city], n, insert_index); // UPDATE

                    printf("for this round the tour is: ");
                        for(int k=0; k<(n+1); k++)
                            printf("%s \t", tour[k]->name);
                        printf("\n");

                    total = min_replaced;
                    printf("~~~the total of this round is %10.2lf\n", total);
                }

                printf("%13.2lf", total);
                for(int i = 0; i < nCity; i++)
                {
                    printf(" %s", tour[i]->name);
                }
                printf(" %s\n", tour[0]->name);

                // Reset bool of all cities to false
                for(int i=0; i<nCity - 1; i++)
                    tour[i]->added = false;
                total = 0;

                double reTotal = 0;
                for(int k = 0; k < nCity-1; k++)
                {
                    //reTotal += location_distance(&(tour[k]->loc), &(tour[k+1]->loc));
                    reTotal += distances[tour[k]->index][tour[k+1]->index];
                }
                reTotal += location_distance(&(tour[0]->loc), &(tour[nCity-1]->loc));
                printf("RECALCULATIOM of TOUR DISTANCE: %10.2lf\n", reTotal);
                break;

            case FARTHEST:
                //printf("-insert farthest  :\n");
                // Now calculate and store the location between points
                double max_dist = location_distance( &(initial_cities[0]->loc), &(initial_cities[1]->loc) );
                
                // Find first two cities & Fill in distances array
                for(int i = 0; i < nCity; ++i) // repeat for the rest of the cities
                {
                    for(int j = 0; j < nCity; ++j)
                    {
                        // UPDATE: initial_cities or ordered_cities?
                        distances[i][j] = location_distance( &(initial_cities[i]->loc), &(initial_cities[j]->loc) );

                        if((max_dist < distances[i][j]) && (i != j))
                        {
                            max_dist = distances[i][j];
                            tour[0] = initial_cities[i]; // store the address of a city
                            tour[1] = initial_cities[j];
                            // tour[1] points to the same address (of a city) as ordered_cities[j]
                        }
                    }
                }
                tour[0]->added = true;
                tour[1]->added = true;
                total += distances[tour[0]->index][tour[1]->index];

                // for the rest of the tour slots
                for(int sad = 0; sad < (nCity-2); ++sad)
                {
                    double minDistToTour[nCity - sad][2];
                    // read in through the distances array
                    for(int i = 0; i < nCity; ++i)
                    {
                        // for cities outside the tour
                        if(!initial_cities[i]->added)
                        {
                            min_replaced = INFINITY;
                            // check distance with cities in the tour
                            for(int j = 0; j < n; ++j)
                            {
                                if(min_replaced > distances[i][j])
                                {
                                    min_replaced = distances[i][j];
                                    minDistToTour[i][0] = j; // store the index of the city the distance belongs to
                                    minDistToTour[i][1] = distances[i][j]; // store the distance from that city to the tour
                                }
                            }
                        }
                    }

                    // now choose the city with max minDistToTour
                    //for(int i=0; i<n-sad) ;
                }
                break;
        }
    }
    // end of statemachine

    for (int i = 0; i < nCity; ++i)
    {
        free(initial_cities[i]->name);
        free(initial_cities[i]);
    }
    free(initial_cities);
    for (int i = 0; i < nCity; ++i) free(ordered_cities[i]);
    free(ordered_cities);
    for (int i = 0; i < nCity; ++i) free(tour[i]);
    free(tour);
    for (int i = 0; i < nCity; ++i) free(distances[i]);
    free(distances);

    return 0;
}




/*
                printf("\nDistances Array: \n");
                for(int i=0; i<nCity; i++)
                {
                    for(int j=0; j<nCity; j++)
                        printf("%10.0lf", distances[i][j]);
                    printf("\n");
                }
*/