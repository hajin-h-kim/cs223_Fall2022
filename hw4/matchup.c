#include "matchup.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct _matchup{
    char* id1;
    char* id2;
} matchup;

typedef enum field_state {ID1, ID2} field_state;

matchup matchup_read(FILE *in, int max_id)
{
    matchup result;
    result.id1 = malloc((max_id+1) * sizeof(char));
    result.id2 = malloc((max_id+1) * sizeof(char));

    int ch;
    int i = 0;
    int j = 0;
    bool error = false;
    field_state state = ID1;
    // what if there aren't any matchups? 
    while((ch = fgetc(in)) != EOF && ch != '\n' && ch != '\r' && !error)
    {
        switch(state)
        {
            case ID1:
                if(i == 32)
                {
                    error = true;
                }
                else if(isspace(ch))
                {
                    state = ID2;
                }
                else
                {
                    result.id1[i++] = ch;
                    result.id1[i] = '\0'; // gets overwritten if the string is longer
                }
                break;
            case ID2:
                if(j == 32)
                {
                    error = true;
                }
                else if(isspace(ch))
                {
                    error = true;
                }
                else
                {
                    result.id1[j++] = ch;
                    result.id1[j] = '\0'; // gets overwritten if the string is longer
                }
                break;
        }
    }
    // is this unecessary? ============
    if(i == 0 || j == 0)
    {
        // an ID (or two) was missing
        error = true;
    }

    if(error)
    {
        free(result.id1);
        result.id1 = NULL;
        free(result.id2);
        result.id2 = NULL;
    }

    return result;
}

void matchup_free(matchup* match)
{
    if(match == NULL) return;
    if(match->id1 != NULL)
    {
        free(match->id1);
        match->id1 = NULL;
    }
    if(match->id2 != NULL)
    {
        free(match->id2);
        match->id2 = NULL;
    }
    return;
}