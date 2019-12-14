/* arraySum.c uses an array to sum the values in an input file,
 *  whose name is specified on the command-line.
 * Joel Adams, Fall 2005
 * for CS 374 (HPC) at Calvin College.
 * 
 * Edited to run Parallel using openMP by Bryce Allen
 * 11/10/19, cs374, proj07
 * 
 * usage: ./arraySum <fileName> <numPE's>
 */

#include <stdio.h>      /* I/O stuff */
#include <stdlib.h>     /* calloc, etc. */
#include <omp.h>
#include <unistd.h>

void readArray(char * fileName, double ** a, int * n);
double sumArray(double * a, int numValues);

int main(int argc, char * argv[])
{
  int  howMany;
  double sum, startTime, endTime, totalTime, ioTime, scatterStart, scatterTime;
  double * a;
  

  if (argc != 3) {
    fprintf(stderr, "\n*** Usage: arraySum <inputFile>\n\n");
    exit(1);
  }

  omp_set_num_threads( atoi(argv[2]) );
  
  startTime = omp_get_wtime();
  readArray(argv[1], &a, &howMany);
  ioTime = omp_get_wtime() - startTime;
  
  scatterStart = omp_get_wtime();
  sum = sumArray(a, howMany);
  scatterTime = omp_get_wtime() - scatterStart;

  endTime = omp_get_wtime();
  totalTime = endTime - startTime;

  printf("The sum of the values in the input file '%s' is %g\n",
           argv[1], sum);
  printf("IOTime: %g\n", ioTime);
  printf("SumTime: %g\n", scatterTime);
  printf("TotalTime: %g\n", totalTime);

  free(a);

  return 0;
}

/********************************************************/

/* readArray fills an array with values from a file.
 * Receive: fileName, a char*,
 *          a, the address of a pointer to an array,
 *          n, the address of an int.
 * PRE: fileName contains N, followed by N double values.
 * POST: a points to a dynamically allocated array
 *        containing the N values from fileName
 *        and n == N.
 */

void readArray(char * fileName, double ** a, int * n) {
  int count, howMany;
  double * tempA;
  FILE * fin;

  fin = fopen(fileName, "r");
  if (fin == NULL) {
    fprintf(stderr, "\n*** Unable to open input file '%s'\n\n",
                     fileName);
    exit(1);
  }

  fscanf(fin, "%d", &howMany);
  tempA = calloc(howMany, sizeof(double));
  if (tempA == NULL) {
    fprintf(stderr, "\n*** Unable to allocate %d-length array",
                     howMany);
    exit(1);
  }

  for (count = 0; count < howMany; count++)
   fscanf(fin, "%lf", &tempA[count]);

  fclose(fin);

  *n = howMany;
  *a = tempA;
}


/* sumArray sums the values in an array of doubles.
 * Receive: a, a pointer to the head of an array;
 *          numValues, the number of values in the array.
 * Return: the sum of the values in the array.
 */

double sumArray(double * a, int numValues) {
  int i;
  double result = 0.0;
  
  #pragma omp parallel for shared(a) reduction(+:result)
  for (i = 0; i < numValues; i++) {
    #pragma omp atomic
    result += a[i];
  }
  return result;
}