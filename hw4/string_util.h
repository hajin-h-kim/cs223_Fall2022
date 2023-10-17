#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

/**
 * Reads from standard input until end of line or file, saving up
 * to the given number of characters in the given string.  The string
 * will be null-terminated.  The newline is not saved in s.
 *
 * @param s a string with space to hold max+1 characters
 * @param max a nonnegative integer
 */
void read_line(char s[], int max);

#endif