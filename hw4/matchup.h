#ifndef __ENTRY_H__
#define __ENTRY_H__

/* Reads in a single line of matchup*/
matchup matchup_read(FILE *in, int max_id);

/* frees a single matchup*/
void matchup_free(matchup* match);

#endif __ENTRY_H__