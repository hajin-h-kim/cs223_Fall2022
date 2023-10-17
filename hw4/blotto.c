#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "gmap.h"
#include "entry.h"
#include "string_util.h"

#define MAX_ID 31

typedef struct _matchup{
    char* id1;
    char* id2;
} matchup;

// function declarations
size_t hash29(const void *key);
void *duplicate(const void *key);
int compare_keys(const void *key1, const void *key2);
void freer(void *key);
void values_destroy(const void *, void *, void *);


// =================================================================================
// Main Functions
// =================================================================================
int main(int argc, char **argv)
{
    // check how many argc there are
    // "there will be at least one command line argument" => you mean ./Blotto ?
    int n_fields = argc-1;
    gmap* m = gmap_create(duplicate, compare_keys, hash29, freer);
    int* nullptr = NULL;
    
    // read in battlefield values from standard input
    int arr[n_fields];
    for(int i = 0; i < n_fields; ++i)
        arr[i] = atoi(argv[i+1]);

    // store distributions
    entry* temp = malloc(sizeof(entry));
    *temp = entry_read(stdin, MAX_ID, n_fields);
    while(!(temp->id == NULL && temp->distribution == NULL))
    {
        // if we reach the end of the file
        if(strcmp(temp->id, "") == 0 && temp->distribution == NULL) break;

        gmap_put(m, temp->id, temp->distribution);
        //entry_destroy(temp);
        // or just the key and not the distribution because we need it later?
        free(temp->id);
        *temp = entry_read(stdin, MAX_ID, n_fields);
    }
    if(temp->id == NULL && temp->distribution == NULL)
    {
        fprintf(stderr, "error: something was wrong in the entry standard input\n");
        entry_destroy(temp); // no point doing this bc id and distribution are already null ptrs
        gmap_for_each(m, values_destroy, nullptr);
        gmap_destroy(m);
        free(temp);
        return 1;
    }
    entry_destroy(temp);
    free(temp);
    
    // store pointers to the strings
    matchup match;
    match.id1 = malloc((MAX_ID+1) * sizeof(char));
    match.id2 = malloc((MAX_ID+1) * sizeof(char));
    
    // two IDs, one empty space in between. not included: null terminator
    int length = MAX_ID*2+1;
    char line[length];
    read_line(line,length);
    
    // make room to deep copy distribution arrays
    int* value1;
    int* value2;

    while(sscanf(line, "%31s %31s", match.id1, match.id2) == 2)
    {
        if(gmap_contains_key(m, match.id1)) value1 = (int*)gmap_get(m, match.id1);
        else
        {
            fprintf(stderr, "error: player %s was not given\n", match.id1);
            free(match.id1);
            free(match.id2);
            gmap_for_each(m, values_destroy, nullptr);
            gmap_destroy(m);
            return 1;
        }
        if(gmap_contains_key(m, match.id2)) value2 = (int*)gmap_get(m, match.id2);
        else
        {
            fprintf(stderr, "error: player %s was not given\n", match.id2);
            free(match.id1);
            free(match.id2);
            gmap_for_each(m, values_destroy, nullptr);
            gmap_destroy(m);
            return 1;
        }

        // check for NULL values to avoid segmentation faults
        if(value1 == NULL || value2 == NULL)
        {
            fprintf(stderr, "error: key does not have associated value\n");
            free(match.id1);
            free(match.id2);
            gmap_for_each(m, values_destroy, nullptr);
            gmap_destroy(m);
            return 1;
        }

        // gmap_get will return an int array
        double total1 = 0;
        double total2 = 0;
        for(size_t ifields = 0; ifields < n_fields; ++ifields)
        {
            if(value1[ifields] > value2[ifields]) total1 += arr[ifields];
            else if(value1[ifields] < value2[ifields]) total2 += arr[ifields];
            else
            {
                total1 += (double)arr[ifields]/2;
                total2 += (double)arr[ifields]/2;
            }
        }

        // print the winner first
        if(total1 >= total2)
          printf("%s %.1lf - %s %.1lf\n", match.id1, total1, match.id2, total2);
        else
          printf("%s %.1lf - %s %.1lf\n", match.id2, total2, match.id1, total1);

        // read in next line
        read_line(line,length);
    }

    // free memory
    free(match.id1);
    free(match.id2);
    gmap_for_each(m, values_destroy, nullptr);
    gmap_destroy(m);
    return 0;
}


// =================================================================================
// Functions to store in gmap and Helper functions
// =================================================================================
size_t hash29(const void *key)
{
  const char *s = key;
  size_t sum = 0;
  size_t factor = 29;
  while (s != NULL && *s != '\0')
    {
      sum += *s * factor;
      s++;
      factor *= 29;
    }

  return sum;
}

void *duplicate(const void *key)
{
  char *s = malloc(strlen(key) + 1);
  if (s != NULL)
    {
      strcpy(s, key);
    }
  return s;
}

int compare_keys(const void *key1, const void *key2)
{
  return strcmp(key1, key2);
}

void freer(void *key)
{
    free(key);
    return;
}

void values_destroy(const void *key, void *value, void * nullptr)
{
  free(value);
  return;
}