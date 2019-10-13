/* mandelMW.c 
 * Compute/draw mandelbrot set using MPI/MPE commands in master/worker
 * format
 *
 * Written Winter, 1998, W. David Laverell.
 *
 * Refactored Winter 2002, Joel Adams. 
 * Copied/edited for cs374 proj04, Calvin Uni, Bryce Allen, October 2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <mpi.h>
#include <mpe.h>
#include "display.h"

/* compute the Mandelbrot-set function for a given
 *  point in the complex plane.
 *
 * Receive: doubles x and y,
 *          complex c.
 * Modify: doubles ans_x and ans_y.
 * POST: ans_x and ans_y contain the results of the mandelbrot-set
 *        function for x, y, and c.
 */
void compute(double x, double y, double c_real, double c_imag,
             double *ans_x, double *ans_y)
{
   *ans_x = x * x - y * y + c_real;
   *ans_y = 2 * x * y + c_imag;
}

/* compute the 'distance' between x and y.
 *
 * Receive: doubles x and y.
 * Return: x^2 + y^2.
 */
double distance(double x, double y)
{
   return x * x + y * y;
}

//-----------------------------------------------------------
int main(int argc, char *argv[])
{
   const int WINDOW_SIZE = 1024;

   short n = 0;
   int ix = 0,
       iy = 0,
       button = 0,
       id = 0;
   int numProcesses = 0;
   short globalMatrix[WINDOW_SIZE][WINDOW_SIZE];
   double spacing = 0.005,
          x = 0.0,
          y = 0.0,
          c_real = 0.0,
          c_imag = 0.0,
          x_center = 1.0,
          y_center = 0.0;

   MPE_XGraph graph;

   double startTime, endTime = 0.0;
   MPI_Status status;
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &id);
   MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

   int numSlices = WINDOW_SIZE / numProcesses;
   short matrix[numSlices][WINDOW_SIZE];
   
   startTime = MPI_Wtime();

   //p0 is master...
   if (id == 0)
   {
      int count = 0;
      for (int i = 0; i < numProcesses; i++) // init first n pe's
      {

         MPI_Send(&globalMatrix[i],
                  WINDOW_SIZE,
                  MPI_SHORT, (id + 1) + i, 1,
                  MPI_COMM_WORLD);

         if (i + 1 == numProcesses) // (i.e on last iteration)
         {
            count = numProcesses;
         } // = numProcesses-1 (8 pes, 7 last i)
      }
      //first n rows have been sent to a process to be computed...
      //ix -> the NEXT row to compute

      MPE_Open_graphics(&graph, MPI_COMM_WORLD,
            getDisplay(),
            -1, -1,
            WINDOW_SIZE, WINDOW_SIZE, 0);

      for (ix = 0; ix < WINDOW_SIZE; ix++)
      {
         MPI_Recv(&globalMatrix[ix],
               WINDOW_SIZE,
               MPI_SHORT, ix + 1,
               MPI_ANY_TAG, MPI_COMM_WORLD,
               &status);
         //printf("%d\n", id);
         for(iy = 0; iy < WINDOW_SIZE; iy++)
         {
            MPE_Draw_point(graph, ix,
                  iy, globalMatrix[ix][iy]);
         }


         //send next row to compute
         if(count < WINDOW_SIZE)
         {
            MPI_Send(&globalMatrix[count],
                  WINDOW_SIZE,
                  MPI_SHORT, (ix%numProcesses), 1,
                  MPI_COMM_WORLD);

            count++;
         }
      }
      endTime = MPI_Wtime() - startTime;
      printf("\nClick in the window to continue...\n");
      printf("\nTime: %f\n", endTime);
      MPE_Get_mouse_press(graph, &ix, &iy, &button);
      MPE_Close_graphics(&graph);
   }
   else//pe > 0
   {
      //short localArray[WINDOW_SIZE];
      int count = 0;

      for( int i = id; i < WINDOW_SIZE; i += id )
      {
         MPI_Recv(&globalMatrix[i],
               WINDOW_SIZE,
               MPI_SHORT, 0,
               MPI_ANY_TAG, MPI_COMM_WORLD,
               &status);
         
         for(int j = 0; j < WINDOW_SIZE; j++)
         {
            c_real = (ix - 400) * spacing - x_center;
            c_imag = (iy - 400) * spacing - y_center;
            x = y = 0.0;
            n = 0;

            while (n < 50 && distance(x, y) < 4.0)
            {
               compute(x, y, c_real, c_imag, &x, &y);
               n++;
            }

            if (n < 50)
            {
               globalMatrix[i][j] = MPE_RED;
            }
            else
            {
               globalMatrix[i][j] = MPE_BLACK;
            }             
         }
         
         MPI_Send(&globalMatrix[i],
               WINDOW_SIZE,
               MPI_SHORT, 0, 1,
               MPI_COMM_WORLD);
      }
   }

   MPI_Finalize();
   return 0;
}
