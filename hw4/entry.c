/* This is a modified version of hw4's optional code, entry.c
 * Updates: covers edge cases:
 * 1. ID with whitespace
 * 2. empty values in distribution
 * 3. calloc result.id and result.distribution instead of malloc
 */
#include "entry.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum parse_state {ID, DISTRIBUTION} parse_state;

entry entry_read(FILE *in, int max_id, int battlefields)
{
  entry result;

  if (in == NULL)
    {
      result.id = NULL;
      result.distribution = NULL;
      return result;
    }

  result.distribution = calloc(battlefields, sizeof(int));
  result.id = calloc((max_id + 1), sizeof(char));

  if (result.distribution == NULL || result.id == NULL)
    {
      // allocation error; abort
      free(result.distribution);
      result.distribution = NULL;
      free(result.id);
      result.id = NULL;
    }
  else
    {
      // initialize FSM to read entry
      int ch;
      parse_state state = ID; // part of entry we're in (ID or DISTRIBUTION)
      int id_len = 0; // chars in id read so far
      result.id[0] = '\0'; 
      int curr_bf = 0; // number of battlefields read so far
      int curr_int = 0; // value of digits in number of units read so far
      int started_int = 0; // boolean flag to remember if a value was read in
      while ((ch = fgetc(in)) != EOF && ch != '\n' && ch != '\r')
        {
          switch (state)
            {
            case ID:
              if (ch == ',')
                {
                  // start reading distribution of units
                  state = DISTRIBUTION;
                }
              else if(isspace(ch))
                {
                  // ID should not have whitespace
                  entry_destroy(&result);
                  return result;
                }
              else if (id_len < max_id)
                {
                  // save character up to max (ignore characters beyond max)
                  result.id[id_len] = ch;
                  id_len++;
                  result.id[id_len] = '\0';
                }
              break;
              
            case DISTRIBUTION:
              if (ch == ',')
                {
                  // the previous distribution slot was empty
                  if(started_int == 0)
                    {
                      entry_destroy(&result);
                      return result;
                    }
                  // comma -- start reading next battlefield
                  curr_bf++;
                  if (curr_bf >= battlefields)
                    {
                      // too many battlefields
                      entry_destroy(&result);
                      return result;
                    }

                  curr_int = 0;
                  started_int = 0; // reinitialize
                }
              else if (!isdigit(ch))
                {
                  // non-comma in integer distribution
                  entry_destroy(&result);
                  return result;
                }
              else
                {
                  // found a proper digit and started reading in the value
                  started_int = 1;
                  // update value of integer we're currently reading
                  curr_int = curr_int * 10 + (ch - '0');
                  result.distribution[curr_bf] = curr_int;
                }
              break;
            }
        }

      // eat carriage-return before line-feed in case some DOS-mode files
      // sneak in
      if (ch == '\r')
        {
          ch = fgetc(in);
        }

      // check that the number of battlefields was correct and if so,
      // whether the id is non-empty
      if ((id_len == 0 && curr_bf > 0)
          || (id_len > 0 && curr_bf != battlefields - 1))
        {
          entry_destroy(&result);
        }
      else if (id_len == 0)
        {
          // empty id and battlefields = 0 means end-of-input
          free(result.distribution);
          result.distribution = NULL;
        }
    }

  return result;
}

void entry_destroy(entry *e)
{
  if (e != NULL)
    {
      free(e->id);
      e->id = NULL;
      free(e->distribution);
      e->distribution = NULL;
    }
}