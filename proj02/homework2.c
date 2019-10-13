/* homework2.c
 * program that demonstrates message passing and master worker
 * parallel strategy
 * Bryce Allen, cs374, proj02, Calvin University
 */
#include <stdio.h>
#include <mpi.h>
#include <string.h>
int main(int argc, char** argv)
{
    int id = -1, numWorkers = -1;
    double startTime = 0.0, totalTime = 0.0;
    int arraySize = 1000*sizeof(char);
    char sendMessage[arraySize];
    char buff[5];
    MPI_Status status;     

    //starts the mpi distributed system parallelization
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id); 
    MPI_Comm_size(MPI_COMM_WORLD, &numWorkers);

    //master (id = 0) worker (id > 0) part
    if ( id == 0 ) {
        startTime = MPI_Wtime();

        //sends a message to process 1
        sprintf(sendMessage, "%d ", id);
        MPI_Send(&sendMessage, 8, MPI_INT, id+1, 1, MPI_COMM_WORLD); 

        //receives a message from process numWorkers - 1
        MPI_Recv(&sendMessage, 256, MPI_INT, numWorkers-1, MPI_ANY_TAG,
             MPI_COMM_WORLD, &status);
        
        //compute time
        totalTime = MPI_Wtime() - startTime;
        
        //master prints to monitor
        printf("%s\n",sendMessage);
        printf("Total time on %d processes: %f\n", numWorkers, totalTime);
    } else { 
        //receives message from id - 1
        MPI_Recv(&sendMessage, 256, MPI_INT, id-1, MPI_ANY_TAG, 
            MPI_COMM_WORLD, &status);

        //create new message
        sprintf(buff, "%d ", id); 
        strcat(sendMessage, buff);
        
        //send message to (id + 1) % numWorkers
        MPI_Send(&sendMessage, 256, MPI_INT, (id+1) % numWorkers, 2, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}