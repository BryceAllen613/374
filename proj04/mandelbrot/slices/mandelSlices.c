/* mandelSlices.c 
 * Compute/draw mandelbrot set using MPI/MPE commands
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
   
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &id);
   MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

   int numSlices = WINDOW_SIZE/numProcesses;
   short matrix[numSlices][WINDOW_SIZE];
   int count = 0;
   startTime = MPI_Wtime();
   for (ix = id; ix < WINDOW_SIZE; ix += numProcesses)
   {
      for (iy = 0; iy < WINDOW_SIZE; iy++)
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
            matrix[count][iy] = MPE_RED;
         }
         else
         {
            matrix[count][iy] = MPE_BLACK;
         }
      }
      count++;
   }
   MPI_Gather(matrix, WINDOW_SIZE * (numSlices), MPI_SHORT,
              globalMatrix, WINDOW_SIZE * (numSlices), MPI_SHORT,
              0, MPI_COMM_WORLD);

   //MPI_Barrier(MPI_COMM_WORLD);

   


   int process_computed_by = 0;
   int process_work_range = numSlices;
   int current_row_to_draw = 0; 

   if (id == 0)
   {
      MPE_Open_graphics(&graph, MPI_COMM_WORLD,
                        getDisplay(),
                        -1, -1,
                        WINDOW_SIZE, WINDOW_SIZE, 0);

      ix = iy = 0;

      for (ix = 0; ix < WINDOW_SIZE; ix++)
      {
         if( ix == process_work_range ) 
         {
            process_work_range += numSlices;
            process_computed_by ++;
            current_row_to_draw = process_computed_by;
         }

         for (iy = 0; iy < WINDOW_SIZE; iy++)
         {
            if( ix < process_work_range )
            { 
               MPE_Draw_point(graph, current_row_to_draw, iy, globalMatrix[ix][iy]);
            }
         }
         current_row_to_draw += numProcesses;
      }
      printf("\nClick in the window to continue...\n");
      endTime = MPI_Wtime() - startTime;
      printf("\nTime: %f\n", endTime);
      MPE_Get_mouse_press(graph, &ix, &iy, &button);
      MPE_Close_graphics(&graph);
   }
   MPI_Finalize();
   return 0;
}
