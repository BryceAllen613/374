/* arraySum.c uses an array to sum the values in an input file,
 *  whose name is specified on the command-line.
 * Joel Adams, Fall 2005
 * for CS 374 (HPC) at Calvin College.
 * 
 * Edited to run Parallel using MPI by Bryce Allen
 * 11/10/19, cs374, proj07
 * 
 * compile: mpicc arraySum.c -Wall -o arraySum
 * usage: mpirun -np 8 -machinefile hosts ./arraySum <filePath> 
 */

#include <stdio.h>      /* I/O stuff */
#include <stdlib.h>     /* calloc, etc. */
#include <string.h>     /* strcat(), etc. */
#include <mpi.h>        /* MPI calls */

void readArray(char * fileName, double ** a, int * n);
double sumArray(double * a, int numValues) ;

MPI_Status status;

int main(int argc, char * argv[])
{
  int id = -1, numWorkers = -1;
  double startTime = 0.0, totalTime = 0.0;

  unsigned howMany;
  double sum = 0.0;
  double * a;
  
  if (argc != 2) {
    fprintf(stderr, "\n*** Usage: arraySum <inputFile>\n\n");
    exit(1);
  }
  
  MPI_Init(&argc, &argv);//INITIALIZE
  MPI_Comm_rank(MPI_COMM_WORLD, &id); 
  MPI_Comm_size(MPI_COMM_WORLD, &numWorkers);

  startTime = MPI_Wtime();//START TIMING

  //read in values and send quantity to all pe's
  if( id == 0 )
  {
    readArray(argv[1], &a, &howMany);
    printf("io time: %g\n", MPI_Wtime() - startTime);
  }
  MPI_Bcast(&howMany, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

  //Each process sums a chunk of a
  unsigned chunkSize = howMany/(numWorkers);
  double * b;
  b = calloc(sizeof(double), chunkSize);
  double curr = MPI_Wtime();
  MPI_Scatter(a, chunkSize, MPI_DOUBLE, 
              b, chunkSize, MPI_DOUBLE, 
              0, MPI_COMM_WORLD);
  if( id == 0 )
  {
    printf("scatter time: %g\n", MPI_Wtime() - curr);
  }
  double localSum = sumArray(b, chunkSize);
  
  //Retrieve the local sums and store in buffer (buff)
  double * buff = NULL;
  if ( id == 0 )
  {
    buff = malloc(sizeof(double) * numWorkers);
  } 
  MPI_Gather(&localSum, 1, MPI_DOUBLE, 
              buff, 1, MPI_DOUBLE, 
              0, MPI_COMM_WORLD);

  //add local sums together
  if( id == 0 )
  {
    sum = sumArray(buff, numWorkers);
  }

  totalTime = MPI_Wtime() - startTime;//END TIMING
  
  if( id == 0 )
  {
    printf("The sum of the values in the input file '%s' is %g\n",
            argv[1], sum);
    printf("Time: %g\n", totalTime);    
  }

  MPI_Finalize();
  return 0;
}


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
  
  for (i = 0; i < numValues; i++) {
    result += *a;
    a++;
  }
  return result;
}

