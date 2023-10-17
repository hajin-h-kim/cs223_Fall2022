#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Reads pairs of integers from the input file with the given name,
 * applies the given function to them, and writes the result to the
 * output file with the given name.
 * 
 * The output file will contain one integer per line with
 * no whitespace other then the newline at the end of each line.
 * 
 * The input will contain one pair of integers per line, with any
 * number of whitespace characters except for newlines separating the
 * two integers and preceeding or following the integers.
 * 
 * You may assume that the input and output filenames refer to
 * different files.  If the output file already exists then it is
 * truncated so its previous contents are replaced with the new
 * output.  If there is an error opening either the input file or the
 * output file then there is no output.  If the input is not as
 * specified then the behavior is undefined, but the function must not
 * crash or hang.
 *
 * Once you're ready to test your code, run ./testit on the command line
 */
// WRITE THE process_file FUNCTION HERE, INCLUDING THE HEADER
void process_file(char* inputFileName, char* outputFileName, int (*pfunc)(int, int))
{
  FILE* inputFile;
  FILE* outputFile;

  inputFile = fopen(inputFileName, "r");
  outputFile = fopen(outputFileName, "w");

  int a,b,c;
  while(fscanf(inputFile, "%d %d", &a, &b) == 2)
  {
    c = (*pfunc)(a, b);
    fprintf(outputFile, "%d\n", c);
  }

  fclose(inputFile);
  fclose(outputFile);
  return;
}
// DO NOT MODIFY THIS LINE. YOU MAY MODIFY SUBSEQUENT LINES, BUT ANY CHANGES MADE WILL NOT BE TAKEN INTO ACCOUNT DURING THE GRADING PROCESS.

int multiply(int, int);
int max(int, int);

int main(int argc, char **argv)
{
  // if numbers.txt contains
  // 2 3
  // 10 8
  // then products.txt should contain
  // 6
  // 80
  // and maximums.txt should contain
  // 3
  // 10
  process_file("numbers.txt", "products.txt", multiply);
  process_file("numbers.txt", "maximums.txt", max);
}

int multiply(int x1, int x2)
{
  return x1 * x2;
}

int max(int x1, int x2)
{ 
  return x1 >= x2 ? x1 : x2;
}


  
