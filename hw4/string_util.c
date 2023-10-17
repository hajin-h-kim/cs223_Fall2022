#include <stdio.h>
#include <ctype.h>

#include "string_util.h"

void read_line(char s[], int max)
{
  int count = 0; // number of chars read
  int ch;
  while ((ch = getchar()) != EOF && ch != '\n')
    {
      if (count < max)
	{
	  s[count] = ch;
	}
      count++;
    }
  s[(count > max ? max : count)] = '\0';
}