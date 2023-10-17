/* This assignment is for CS223 pset1 
 * It reades from a GPX file to extract the location information
 * stored, and writes to standard output*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    PLAINTEXT, // for finding <
    TRACKPOINT, // for finding trkpt
    ATTRIBUTE, // for finding lat, lon
    ELEVATION, // for finding ele
    TIME, // for finding time
    PRINT // for printing all outputs
} state_type;

int main(int argc, char **argv)
{
    state_type state = PLAINTEXT; // initialize
    int c; // int to read in a single character
    int count = 0; // number of characters read into tempString array
    int quote = 0; // remembers whether we are in quotes (used for attributes)
    int flag_trkpt = 0; // looking for.. trkpt: 0, lat: 1, lon: 2, ele: 3, time: 4
    int ended_start_tag = 0; // 0 if false, 1 if true
    char tempString[7]; // checks for trkpt, lat, lon, ele, time

    // use one big while loop to read in characters
    while( (c = fgetc(stdin)) != EOF)
    {
        switch (state)
        {
            case PLAINTEXT:
                // '<' only appears at the start of a tag
                if( c == '<' )
                {
                    if(flag_trkpt == 0) state = TRACKPOINT;
                    else if(flag_trkpt == 3) state = ELEVATION;
                    else if(flag_trkpt == 4) state = TIME;
                }
                break;

            case TRACKPOINT:
                // read in 5 characters of trkpt and an empty space
                if(count<6)
                {
                    tempString[count] = tolower(c);
                    count++; // number of chars read into the array
                }

                // done reading element
                if(isspace(c) || c == '>' || count >= 6)
                {
                    tempString[6] = '\0';                    
                    // if the strings are equal, we've found trkpt
                    if(strcmp(tempString, "trkpt ") == 0)
                    {
                        state = ATTRIBUTE;
                        flag_trkpt++;
                    }
                    // if not, go back to searching for trkpt
                    else
                        state = PLAINTEXT;                   
                    // empty buffers
                    count = 0;
                    strcpy(tempString, "");
                }
                break;

            case ATTRIBUTE:
                // remember if we are in quotes
                // we will only check attribute names when we are outside of quotes,
                // which we will remember with the boolean 'quote'
                if(c == '\'' || c == '\"')
                {
                    if(quote == 0) // was not in quotes
                        quote = c;
                    else if(quote == c) // was in, met same quote
                        quote = 0; // finished quote
                    break;
                }
                // start taking in attribute name when we are...
                // outside quotes, and met a nonwhite character
                if(quote == 0 && !isspace(c) && count == 0) // outside quotes, start taking in attribute
                {
                    tempString[count] = c;
                    count++;
                    break;
                }
                // after we have started taking in char for the attribute name, read until...
                // meeting whitespace, or meeting '=', or reading all the 4 characters needed
                if(quote == 0 && ((0<count && count<=3) || !isspace(c) || c != '=' )) // continue reading attribute
                {
                    tempString[count] = tolower(c);
                    count++;
                }
                // finished reading attribute name
                // we read in one more character to check for errors such as "latt"
                if(quote == 0 && (count > 3 || isspace(c) || c == '=' ))
                {
                    // terminate char array with null character for strcmp
                    tempString[count] = '\0';
                    switch (flag_trkpt)
                    {
                        // looking for lat
                        case 1:
                            if(strcmp(tempString, "lat ")==0 || strcmp(tempString, "lat=")==0)
                                state = PRINT;
                            break;
                        // looking for lon
                        case 2:
                            if(strcmp(tempString, "lon ")==0 || strcmp(tempString, "lon=")==0)
                                state = PRINT;
                            break;
                    }
                    // empty buffers
                    strcpy(tempString, "");
                    count = 0;
                }
                break;

            // same design as case TRACKPOINT; only difference is the # of characters read in
            case ELEVATION:
                if(count < 4)
                {
                    tempString[count] = tolower(c);
                    count++; // number of chars read in
                }
                if(count > 4|| isspace(c) || c == '>') // done reading element
                {
                    // terminate char array with null character for strcmp
                    tempString[4] = '\0';
                    if(strcmp(tempString, "ele ")==0 || strcmp(tempString, "ele>")==0)
                    {
                        if(c == '>') ended_start_tag = 1;
                        state = PRINT;
                    }
                    else
                    {
                        state = PLAINTEXT; // to find <
                    }
                    // empty buffer
                    strcpy(tempString, "");
                    count = 0;
                }
                break;

            // same design as case TRACKPOINT and ELEVATION; only difference is the # of characters read in
            case TIME:
                if(count < 5)
                {
                    tempString[count] = tolower(c);
                    count++; // number of chars read in
                }
                if(count > 5|| isspace(c) || c == '>') // done reading element
                {
                    // terminate char array with null character for strcmp
                    tempString[5] = '\0';
                    if(strcmp(tempString, "time ")==0 || strcmp(tempString, "time>")==0)
                    {
                        if(c == '>') ended_start_tag = 1;
                        state = PRINT;
                    }
                    else
                    {
                        state = PLAINTEXT; // to find <
                    }
                    // empty buffer
                    strcpy(tempString, "");
                    count = 0;
                }
                break;
            
            case PRINT:
                switch (flag_trkpt)
                {
                    // attribute cases
                    case 1: case 2:
                        if(c == '\'' || c == '\"')
                        {
                            if(quote == 0) // was not in an attribute value
                                quote = c;
                            else if(quote != c) // was in, met different quote
                            {
                                if(c == ',') printf("&comma;"); // replace commas
                                else fputc(c, stdout);
                            }
                            else if(quote == c) // was in, met same quote
                            {
                                quote = 0; // finished quote
                                printf(",");
                                if(flag_trkpt == 1) state = ATTRIBUTE;
                                if(flag_trkpt == 2) state = PLAINTEXT;
                                flag_trkpt++;
                            }
                            break;
                        }
                        if(quote != 0) // inside quotes
                        {
                            fputc(c, stdout);
                            break;
                        }
                    
                    // child element cases
                    case 3: case 4:
                        // delay printing elevation and time
                        // until reaching '>', the end of the start tag
                        if(c == '>')
                        {
                            ended_start_tag = 1;
                            break;
                        }

                        // waited until finding c == '>', now print 
                        if(ended_start_tag)
                        {
                            // until we find '<', the start of the end tag
                            if(c != '<')
                            {
                                if(c == ',') printf("&comma;"); // replace commas
                                else fputc(c, stdout);
                            }
                            // if we find '<', finish up printing state
                            else
                            {
                                ended_start_tag = 0;
                                if(flag_trkpt == 3)
                                {
                                    printf(",");
                                    state = PLAINTEXT; // to find '<' for time element
                                    flag_trkpt++;
                                }
                                else if(flag_trkpt == 4)
                                {
                                    fputc('\n', stdout);
                                    state = PLAINTEXT;
                                    flag_trkpt = 0; // initialize trackpoint flag
                                }
                            }
                        }
                        break;
                }
                break; 
        }
    }
    return 0;
}